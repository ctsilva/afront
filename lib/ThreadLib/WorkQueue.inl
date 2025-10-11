// #include "WorkQueue.h"

BEGIN_THREADLIB_NAMESPACE

template <class OBJECT_TYPE, class CONSUMER>
WorkQueue<OBJECT_TYPE,CONSUMER>::WorkQueue(int num_threads,  typename CONSUMER consumer, int priority) :
    _consumer(consumer),
    _thread_counter(num_threads),
    _progress_resolution(-1),
    _num_non_idle_threads(0)
{ 

    _queue_items_semaphore = CreateSemaphore(NULL, 0, 2000000000, NULL);
    _shutdown_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    _shutdown_completed_event = CreateEvent(NULL, TRUE, FALSE, NULL);
    _empty_queue_handle = CreateEvent(NULL, TRUE, TRUE, NULL);

    for (int i = 0; i < num_threads; ++i)
    {
        HANDLE h = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_entry, this, 0, NULL);
        SetThreadPriority(h, priority);
        CloseHandle(h);
    }
}

template <class OBJECT_TYPE, class CONSUMER>
WorkQueue<OBJECT_TYPE,CONSUMER>::~WorkQueue()
{
    SetEvent(_shutdown_event);
    WaitForSingleObject(_shutdown_completed_event, INFINITE);
    CloseHandle(_queue_items_semaphore);
    CloseHandle(_shutdown_event);
    CloseHandle(_shutdown_completed_event);
    CloseHandle(_empty_queue_handle);
}

template <class OBJECT_TYPE, class CONSUMER>
void WorkQueue<OBJECT_TYPE,CONSUMER>::InsertItem(gtb::sptr<OBJECT_TYPE> item)
{
    CS ___cs(_work_queue_guard);
    _work_queue.push_back(item);
    ResetEvent(_empty_queue_handle);
    ReleaseSemaphore(_queue_items_semaphore, 1, NULL);
}

template <class OBJECT_TYPE, class CONSUMER>
void WorkQueue<OBJECT_TYPE,CONSUMER>::consumer_thread()
{
    HANDLE waithandles[2] = {_queue_items_semaphore, _shutdown_event};
    DWORD id;
    do
    {
        id = WaitForMultipleObjects(2, waithandles, FALSE, INFINITE);
        if (id == WAIT_OBJECT_0)
        {
            _work_queue_guard.enter();
            unsigned qsize = _work_queue.size();
            if (qsize == 0) printf("Bug: queue size =0\n");
            assert(qsize > 0);

            if ((_progress_resolution > 0) && (qsize % _progress_resolution == 0))
            {
                printf("Q: %d        \r", qsize);
            }

            t_work_queue::value_type item = _work_queue.front();
            _work_queue.pop_front();
            ++_num_non_idle_threads;
            _work_queue_guard.leave();

            _consumer(item);

            //
            // If this was the last item, signal the empty queue event
            //
            _work_queue_guard.enter();
            --_num_non_idle_threads;
            if ((_work_queue.size() == 0) && (_num_non_idle_threads == 0))
            {
                SetEvent(_empty_queue_handle);
            }
            _work_queue_guard.leave();
        }
    } while (id != WAIT_OBJECT_0+1);
    --_thread_counter;

    //
    // The last thread closes the lights...
    // i.e. lets the destructor return
    //
    if (_thread_counter == 0) SetEvent(_shutdown_completed_event);
}

template <class OBJECT_TYPE, class CONSUMER>
void WorkQueue<OBJECT_TYPE,CONSUMER>::thread_entry(WorkQueue* qobject)
{
    qobject->consumer_thread();
}

template <class OBJECT_TYPE, class CONSUMER>
void WorkQueue<OBJECT_TYPE,CONSUMER>::WaitForListToClear()
{
    WaitForSingleObject(_empty_queue_handle, INFINITE);
}

template <class OBJECT_TYPE, class CONSUMER>
void WorkQueue<OBJECT_TYPE,CONSUMER>::show_progress(int resolution)
{
    _progress_resolution = resolution;
}

END_THREADLIB_NAMESPACE
