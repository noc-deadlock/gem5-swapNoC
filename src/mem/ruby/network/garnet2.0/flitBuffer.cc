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


#include "mem/ruby/network/garnet2.0/flitBuffer.hh"
using namespace std;

flitBuffer::flitBuffer()
{
    max_size = INFINITE_;
}

flitBuffer::flitBuffer(int maximum_size)
{
    max_size = maximum_size;
}

bool
flitBuffer::isEmpty()
{
    return (m_buffer.size() == 0);
}

int
flitBuffer::bufferSizeLeft()
{
    return (max_size - m_buffer.size());
}

bool
flitBuffer::swap(int outport)
{
    //Do swapping here
//    cout << "[ flitBuffer.cc: " <<__LINE__ <<"] Doing Swap here" << endl;
//    int pos = m_buffer.size();
//    iter_swap(m_buffer.begin(), m_buffer.begin()+(pos-1));
    // scan through all the flits in the queue...
    for (std::vector<flit *>::iterator i = m_buffer.begin(); i != m_buffer.end(); ++i) {
        if((*i)->get_outport() == outport) {
            // do the swap...
            if((*(m_buffer.end()-1))->get_outport() != outport)
                iter_swap(m_buffer.end()-1, i);
            return true;
        }
    }
    return false;
}

void
flitBuffer::rand_swap()
{
    // Randomly pic any position and swap it with HoL
    int pos = random() % m_buffer.size();
    std::vector<flit *>::iterator i = m_buffer.begin() + pos;
    iter_swap(m_buffer.begin(), i);
    return;
}

// TODO: Only swap when there is no credit available fo that port
// Don't do it instead of reaching the threshold...
// That is, don't call this function as the flit as coming....
void
flitBuffer::intel_swap()
{
    // Parse through the buffer and do swap of HoL
    // with the first one having Outport different
    // then HoL
//    for (std::vector<flit *>::iterator i = m_buffer.begin(); i != m_buffer.end(); ++i) {
//        if((*i)->get_outport() != m_buffer.front()->get_outport()) {
//            int pos = i - m_buffer.begin();
//            iter_swap(m_buffer.begin(), m_buffer.begin()+(pos));
//            return; // you have done the swapping.. Just return.
//        }
//    }

    // We should instead parse through buffer from its tail to head... and if
    // we find a flit whose outport is different than HoL then do the swap!

    for (std::vector<flit *>::iterator i = m_buffer.end()-1; i >= m_buffer.begin(); --i) {
        if((*i)->get_outport() != m_buffer.front()->get_outport()) {
            //TODO: How to find the correct position... current logic looks fine...
            int pos = i - m_buffer.begin();
            iter_swap(m_buffer.begin(), m_buffer.begin()+(pos));
            return; // you have done the swapping.. Just return.
        }
    }
}


void
flitBuffer::tail_swap()
{
   // swap the head of the queue with its tail. No loop required.
    iter_swap(m_buffer.begin(), m_buffer.end()-1); // this is for swapping tail and head
}

bool
flitBuffer::scan_queue()
{
    // Note: I think below logic is not correct to determine HoL
    // Parse through the buffer and return true
    // if there's any packet whoes outport is
    // different than the HoL.
    for (std::vector<flit *>::iterator i = m_buffer.begin(); i != m_buffer.end(); ++i) {
        if((*i)->get_outport() != m_buffer.front()->get_outport()) {
            return (true);
        }
    }
    // if control comes here.. it hasn't found any packet which
    // has outport different than HoL.
    return (false);
}

bool
flitBuffer::isReady(Cycles curTime)
{
    if (m_buffer.size() != 0 ) {
        flit *t_flit = peekTopFlit();
        if (t_flit->get_time() <= curTime)
            return true;
    }
    return false;
}

void
flitBuffer::print(std::ostream& out) const
{
    out << "[flitBuffer: " << m_buffer.size() << "] " << std::endl;
}

bool
flitBuffer::isFull()
{
    // this is the issue... hack to check if swap works!
    return (m_buffer.size() >= max_size);
    // return (m_buffer.size() >= 4/*max_size*/);
}

bool
flitBuffer::isThreshold(int threshold)
{
    return(m_buffer.size() >= threshold);
}

void
flitBuffer::setMaxSize(int maximum)
{
    max_size = maximum;
}

uint32_t
flitBuffer::functionalWrite(Packet *pkt)
{
    uint32_t num_functional_writes = 0;

    for (unsigned int i = 0; i < m_buffer.size(); ++i) {
        if (m_buffer[i]->functionalWrite(pkt)) {
            num_functional_writes++;
        }
    }

    return num_functional_writes;
}
