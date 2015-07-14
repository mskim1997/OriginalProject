using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace Control
{
    public class BlockingQueue<T>
    {
        private Queue<T> queue = new Queue<T>();

        private Semaphore sem = new Semaphore(0, int.MaxValue);

        private object mutex = new object();

        private int waitingThreadCount;

        private bool isTerminated;

        public BlockingQueue()
        {
        }

        public void Enqueue(T item)
        {
            lock (this.mutex)
            {
                if (!this.isTerminated)
                {
                    queue.Enqueue(item);
                    sem.Release();
                }
            }
        }

        public T Dequeue()
        {
            lock (this.mutex)
            {
                waitingThreadCount++;
            }

            sem.WaitOne();

            lock (this.mutex)
            {
                waitingThreadCount--;
            }

            lock (this.mutex)
            {
                if (!this.isTerminated)
                {
                    return queue.Dequeue();
                }
                else
                {
                    return default(T);
                }
            }
        }

        public void Terminate()
        {
            lock (this.mutex)
            {
                this.isTerminated = true;
                sem.Release(waitingThreadCount);
            }
        }
    }
}
