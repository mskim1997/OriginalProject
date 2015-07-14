using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Control.AgentService;
using System.ServiceModel;

namespace Control
{
    class AgentServiceCallback : IAgentServiceCallback
    {
        public AgentServiceCallback()
        {
            this.ClientInstanceContext = new InstanceContext(this);
        }

        public event EventHandler<AgentPosition> ClientCallback;

        public InstanceContext ClientInstanceContext
        {
            get;
            private set;
        }

        #region IAgentServiceCallback Members

        public void Inform(Control.AgentService.AgentPosition pos)
        {
            if (this.ClientCallback != null)
            {
                this.ClientCallback(this, pos);
            }
        }

        #endregion
    }
}
