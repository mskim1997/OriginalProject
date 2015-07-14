using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Threading;
using Control.AgentService;

namespace Control
{
    public class MapRow : INotifyPropertyChanged
    {
        private char[] column;
        public MapRow(int size)
        {
            this.column = new char[size];
        }

        public char[] Column
        {
            get
            {
                return this.column;
            }
            set
            {
                this.column = value;
                NotifyPropertyChanged("Column");
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        private void NotifyPropertyChanged(string info)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(info));
            }
        }
    }

    public class Controller
    {        
        private List<AgentPosition> goalList = new List<AgentPosition>();
        private BlockingQueue<AgentPosition> occupiedPositionList = new BlockingQueue<AgentPosition>();
        private BlockingQueue<AgentPosition> availablePositionList = new BlockingQueue<AgentPosition>();
        private object mutex = new object();
        private AgentServiceCallback callback = new AgentServiceCallback();
        private AgentServiceClient remoteService;

        public ObservableCollection<MapRow> AgentCollection
        {
            get;
            set;
        }

        public int XSize
        {
            get;
            set;
        }

        public int YSize
        {
            get;
            set;
        }

        public bool Terminate
        {
            get;
            private set;
        }

        public Controller()
        {
            YSize = 12;
            XSize = 8;

            this.AgentCollection = new ObservableCollection<MapRow>();

            for (int i = 0; i < YSize; i++)
            {
                this.AgentCollection.Add(new MapRow(XSize));

                for (int j = 0; j < XSize; j++)
                {
                    this.AgentCollection[i].Column[j] = '?';
                }
            }            

            this.callback.ClientCallback += Update;
            this.remoteService = new AgentServiceClient(this.callback.ClientInstanceContext);
            this.remoteService.RegisterClient();

            //ThreadPool.QueueUserWorkItem(TestThread);
            ThreadPool.QueueUserWorkItem(ReaderThread);
            ThreadPool.QueueUserWorkItem(CleanerThread);
        }

        public void Close()
        {
            this.Terminate = true;
            this.occupiedPositionList.Terminate();
            this.availablePositionList.Terminate();
        }

        private void Update(object sender, AgentPosition pos)
        {
            this.occupiedPositionList.Enqueue(pos);
        }
        
        //private void TestThread(object arg)
        //{            
        //    int x = 0;
        //    int y = 0;
        //    char Object1 = '1';
        //    char Object2 = '2';

        //    while (!this.Terminate)
        //    {
        //        this.occupiedPositionList.Enqueue(new AgentPosition(Object1, (x + 1)%this.XSize, y));
        //        this.occupiedPositionList.Enqueue(new AgentPosition(Object2, x, y));

        //        x++;

        //        if (x == this.XSize)
        //        {
        //            x = 0;
        //            y++;

        //            if (y == this.YSize)
        //            {
        //                y = 0;
        //            }
        //        }

        //        Thread.Sleep(500);
        //    }
        //}

        private void AddObject(AgentPosition newObject)
        {
            char[] tempRow = this.AgentCollection[newObject.YPos].Column;
            tempRow[newObject.XPos] = (char)newObject.Object;

            this.AgentCollection[newObject.YPos].Column = tempRow;
        }

        private void ReaderThread(object arg)
        {
            AgentPosition newPos = null;

            while (!this.Terminate)
            {
                newPos = this.occupiedPositionList.Dequeue();

                if (newPos != null)
                {
                    lock (this.mutex)
                    {
                        this.availablePositionList.Enqueue(newPos);
                        AddObject(newPos);
                    }
                }
            }
        }

        private void CleanerThread(object arg)
        {
            AgentPosition newPos = null;
            AgentPosition goalPos = null;

            while (!this.Terminate)
            {
                newPos = this.availablePositionList.Dequeue();

                if (newPos != null)
                {
                    if (newPos.Object != 'B')
                    {
                        lock (this.mutex)
                        {
                            for (int i = 0; i < this.YSize; i++)
                            {
                                for (int j = 0; j < this.XSize; j++)
                                {
                                    if (newPos.Object == this.AgentCollection[i].Column[j])//this.charMap[i][j])
                                    {
                                        if ((i == newPos.YPos) && (j == newPos.XPos))
                                        {
                                        }
                                        else
                                        {
                                            goalPos = this.goalList.Find(pos => ((pos.YPos == i) && (pos.XPos == j)));

                                            if (goalPos != null)
                                            {
                                                AddObject(goalPos);
                                            }
                                            else
                                            {
                                                goalPos = new AgentPosition();
                                                goalPos.Object = (sbyte)'O';
                                                goalPos.XPos = j;
                                                goalPos.YPos = i;

                                                AddObject(goalPos);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
