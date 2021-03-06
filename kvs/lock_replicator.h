// Copyright (c) 2015-2016, Robert Escriva, Cornell University
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//     * Redistributions of source code must retain the above copyright notice,
//       this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of Consus nor the names of its contributors may be
//       used to endorse or promote products derived from this software without
//       specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef consus_kvs_lock_replicator_h_
#define consus_kvs_lock_replicator_h_

// po6
#include <po6/threads/mutex.h>

// e
#include <e/slice.h>

// consus
#include <consus.h>
#include "namespace.h"
#include "common/ids.h"
#include "common/lock.h"
#include "common/transaction_group.h"

BEGIN_CONSUS_NAMESPACE
class daemon;

class lock_replicator
{
    public:
        lock_replicator(uint64_t key);
        virtual ~lock_replicator() throw ();

    public:
        uint64_t state_key();
        bool finished();

    public:
        void init(comm_id id, uint64_t nonce,
                  const e::slice& table, const e::slice& key,
                  const transaction_group& tg, lock_op op,
                  std::auto_ptr<e::buffer> backing);
        void response(comm_id id, const transaction_group& tg,
                      const replica_set& rs, daemon* d);
        void abort(const transaction_group& tg, daemon* d);
        void drop(const transaction_group& tg);
        void externally_work_state_machine(daemon* d);
        std::string debug_dump();

    private:
        struct lock_stub;

    private:
        std::string logid();
        lock_stub* get_stub(comm_id id);
        lock_stub* get_or_create_stub(comm_id id);
        void ensure_stub_exists(comm_id id) { get_or_create_stub(id); }
        void work_state_machine(daemon* d);
        void send_lock_request(lock_stub* stub, uint64_t now, daemon* d);

    private:
        const uint64_t m_state_key;
        po6::threads::mutex m_mtx;
        bool m_init;
        bool m_finished;
        comm_id m_id;
        uint64_t m_nonce;
        e::slice m_table;
        e::slice m_key;
        transaction_group m_tg;
        lock_op m_op;
        std::auto_ptr<e::buffer> m_backing;
        std::vector<lock_stub> m_requests;
};

END_CONSUS_NAMESPACE

#endif // consus_kvs_lock_replicator_h_
