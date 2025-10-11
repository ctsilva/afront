#ifndef __WORKQUEUE_H
#define __WORKQUEUE_H

#include <gtb/memory/ptrs.h>
#include "CriticalSection.h"

BEGIN_THREADLIB_NAMESPACE

/*
 * Work queue class
 * A producer enters object to be processed by a consumer
 * The class also creates multiple consumer threads.
 *
 * Template arguments:
 *   OBJECT_TYPE - the type of objects to process
 *   CONSUMER - a function object that accepts OBJECT_TYPE
 */
template <class OBJECT_TYPE, class CONSUMER>
class WorkQueue
{
public:
    WorkQueue(int num_threads, typename CONSUMER consumer, int priority = THREAD_PRIORITY_NORMAL);
    ~WorkQueue();

    void InsertItem(gtb::sptr<OBJECT_TYPE> item);

    //
    // Wait until the work queue is empty
    //
    void WaitForListToClear();

    void consumer_thread();

    //
    // Print the size of the queue at every "resolution"
    // i.e. if q.size()%resolution == 0
    //
    void show_progress(int resolution);

private:
    CONSUMER _consumer;
    typedef std::deque<gtb::sptr<OBJECT_TYPE> > t_work_queue;
    t_work_queue _work_queue;
    CSObject _work_queue_guard;
    unsigned _progress_resolution;

    HANDLE _queue_items_semaphore; // a semaphor that is release for
                                   // each item in the queue

    HANDLE _shutdown_event;
    HANDLE _shutdown_completed_event; // This semaphor is signaled
    // by the last thread that exits (there must be a nicer
    // way)
    gtb::Counter _thread_counter;

    //
    // Event handle that is signaled if the work queue is empty
    //
    HANDLE _empty_queue_handle;
    int _num_non_idle_threads; // the above event is set only
                               // after the last item was comsumed
                               // and processed

    static void thread_entry(WorkQueue* qobject);
};

END_THREADLIB_NAMESPACE

#include "WorkQueue.inl"


#endif // __WORKQUEUE_H
