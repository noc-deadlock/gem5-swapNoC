/*
 * Copyright (c) 2008 Princeton University
 * Copyright (c) 2016 Georgia Institute of Technology
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Niket Agarwal
 *          Tushar Krishna
 */


#include "mem/ruby/network/garnet2.0/InputUnit.hh"

#include "base/stl_helpers.hh"
#include "debug/RubyNetwork.hh"
#include "mem/ruby/network/garnet2.0/Credit.hh"
#include "mem/ruby/network/garnet2.0/Router.hh"

using namespace std;
using m5::stl_helpers::deletePointers;

InputUnit::InputUnit(int id, PortDirection direction, Router *router)
            : Consumer(router)
{
    m_id = id;
    m_direction = direction;
    m_router = router;
    m_num_vcs = m_router->get_num_vcs();
    m_vc_per_vnet = m_router->get_vc_per_vnet();
    m_output_unit = m_router->get_outputUnit_ref();  // using here to get credit count

    m_num_buffer_reads.resize(m_num_vcs/m_vc_per_vnet);
    m_num_buffer_writes.resize(m_num_vcs/m_vc_per_vnet);
    for (int i = 0; i < m_num_buffer_reads.size(); i++) {
        m_num_buffer_reads[i] = 0;
        m_num_buffer_writes[i] = 0;
    }

    creditQueue = new flitBuffer();
    // Instantiating the virtual channels
    m_vcs.resize(m_num_vcs);
    for (int i=0; i < m_num_vcs; i++) {
        m_vcs[i] = new VirtualChannel(i);
    }
}

InputUnit::~InputUnit()
{
    delete creditQueue;
    deletePointers(m_vcs);
}

/*
 * The InputUnit wakeup function reads the input flit from its input link.
 * Each flit arrives with an input VC.
 * For HEAD/HEAD_TAIL flits, performs route computation,
 * and updates route in the input VC.
 * The flit is buffered for (m_latency - 1) cycles in the input VC
 * and marked as valid for SwitchAllocation starting that cycle.
 *
 */

void
InputUnit::wakeup()
{
    flit *t_flit;
    if (m_in_link->isReady(m_router->curCycle())) {

        t_flit = m_in_link->consumeLink();
        int vc = t_flit->get_vc();
        t_flit->increment_hops(); // for stats

        if ((t_flit->get_type() == HEAD_) ||
            (t_flit->get_type() == HEAD_TAIL_)) {

            if(!(m_router->get_net_ptr()->isEnableWormhole()))
                assert(m_vcs[vc]->get_state() == IDLE_);
            set_vc_active(vc, m_router->curCycle());

            // Route computation for this vc
            int outport = m_router->route_compute(t_flit->get_route(),
                m_id, m_direction);

            // Update output port in VC
            // All flits in this packet will use this output port
            // The output port field in the flit is updated after it wins SA
            grant_outport(vc, outport);
            if(m_router->get_net_ptr()->isEnableWormhole()) {
                //set output port in this flit as well
                t_flit->set_outport(outport);

                //set outvc to 0
                t_flit->set_vc(0);                
                assert(m_vcs[vc]->get_state() == ACTIVE_);
            }

        } else {
            assert(m_vcs[vc]->get_state() == ACTIVE_);
        }

       if(m_router->get_net_ptr()->isEnableWormhole()) {

            assert(vc==0);
            // Not full
            assert(m_vcs[0]->size() < m_router->get_net_ptr()->getBuffersPerCtrlVC());
        }


        // Buffer the flit
        m_vcs[vc]->insertFlit(t_flit);

        int vnet = vc/m_vc_per_vnet;
        // number of writes same as reads
        // any flit that is written will be read only once
        m_num_buffer_writes[vnet]++;
        m_num_buffer_reads[vnet]++;
        // do the swap here...
        if(m_router->get_net_ptr()->isEnableIntraswap()) {
            if (m_vcs[0]->isThreshold(m_router->get_net_ptr()->threshold())==true) {
                // Instead of threshold.. do it on credit avaliablility basis of HoL flit
                flit *f = m_vcs[0]->peekTopFlit(); // this is HoL flit...
                int outport = f->get_outport();

                bool credit = m_router->hasCredit(outport, 0);
                if(credit == false) {
                  if (m_vcs[0]->isThreshold(m_router->get_net_ptr()->threshold())==true) {
                    m_router->get_net_ptr()->m_hol_total++; // this is hol blocking
                    // DO SWAPPING HERE
                    // m_vcs[0]->swap();
                    // DO intelligent swap; instead of swapping it with the last flit
                    // swap HoL with the next flit that has differnt outport then HoL
//                    m_vcs[0]->intel_swap();
                     // m_vcs[0]->tail_swap();
                    // Check if intel_swap() resolved hol()
                    f = m_vcs[0]->peekTopFlit(); // this is new HoL flit
                    credit = m_router->hasCredit(outport, 0/*outvc*/);
                    if(credit)
                        m_router->get_net_ptr()->m_hol_avoidable++;
                    else
                        m_router->get_net_ptr()->m_hol_blocked++;
                  } else {
                        m_router->get_net_ptr()->m_no_hol++;
                  }
                } else {
                    // nothing here...
                }
            } else {
                // nothing here...
            }
        }

        Cycles pipe_stages = m_router->get_pipe_stages();
        if (pipe_stages == 1) {
            // 1-cycle router
            // Flit goes for SA directly
            t_flit->advance_stage(SA_, m_router->curCycle());
        } else {
            assert(pipe_stages > 1);
            // Router delay is modeled by making flit wait in buffer for
            // (pipe_stages cycles - 1) cycles before going for SA

            Cycles wait_time = pipe_stages - Cycles(1);
            t_flit->advance_stage(SA_, m_router->curCycle() + wait_time);

            // Wakeup the router in that cycle to perform SA
            m_router->schedule_wakeup(Cycles(wait_time));
        }
    }
}


// check_hol_outport(int outport) to check if there's any input queue
// with hol of same outport. if yes, then set done = 1
void
InputUnit::check_hol_outport(int outport) {
    flit *f = m_vcs[0]->peekTopFlit(); // this is hol flit of curr input port
    if (outport == f->get_outport())
        m_router->done = 1;
    else
        m_router->done = 0;
}

// shuffle_inport(int inport): pick a random flit from the inport and swap
// it with hol; thus making it HoL.
void
InputUnit::shuffle_inport() {
    m_vcs[0]->rand_swap();
}


void
InputUnit::swap_back(int outport) {
    m_vcs[0]->swap(outport);
}

// check_hol_block(int outport) to check if there's any input queue which
// is hol_blocked, if yes, swap with a flit having the same outport as in
// the argument. Return true (done=1) if successful, else return false (done=0)
void
InputUnit::check_hol_block(int outport){
    flit *f = m_vcs[0]->peekTopFlit(); // this is HoL flit...
    int hol_outport = f->get_outport();
    bool credit = m_router->hasCredit(hol_outport, 0); // this to check if there is hol block or not
    if (credit == true)
        m_router->done = 0;
    else {
        // there's hol-block... check for a successful swap..
        bool result = m_vcs[0]->swap(outport);
        if (result == true) // this means swap is successful; set done = 1
            m_router->done = 1;
        else // swap was unsuccessful, go to next inputport...
            m_router->done = 0;
    }


}

// Send a credit back to upstream router for this VC.
// Called by SwitchAllocator when the flit in this VC wins the Switch.
void
InputUnit::increment_credit(int in_vc, bool free_signal, Cycles curTime)
{
    Credit *t_credit = new Credit(in_vc, free_signal, curTime);
    creditQueue->insert(t_credit);
    m_credit_link->scheduleEventAbsolute(m_router->clockEdge(Cycles(1)));
}


uint32_t
InputUnit::functionalWrite(Packet *pkt)
{
    uint32_t num_functional_writes = 0;
    for (int i=0; i < m_num_vcs; i++) {
        num_functional_writes += m_vcs[i]->functionalWrite(pkt);
    }

    return num_functional_writes;
}

void
InputUnit::resetStats()
{
    for (int j = 0; j < m_num_buffer_reads.size(); j++) {
        m_num_buffer_reads[j] = 0;
        m_num_buffer_writes[j] = 0;
    }
}
