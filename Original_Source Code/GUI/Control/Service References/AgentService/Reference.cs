﻿//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:2.0.50727.4016
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace Control.AgentService {
    using System.Runtime.Serialization;
    using System;
    
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.Runtime.Serialization", "3.0.0.0")]
    [System.Runtime.Serialization.DataContractAttribute(Name="AgentPosition", Namespace="http://schemas.datacontract.org/2004/07/")]
    [System.SerializableAttribute()]
    public partial class AgentPosition : System.EventArgs, System.Runtime.Serialization.IExtensibleDataObject, System.ComponentModel.INotifyPropertyChanged {
        
        [System.NonSerializedAttribute()]
        private System.Runtime.Serialization.ExtensionDataObject extensionDataField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private sbyte ObjectField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private int XPosField;
        
        [System.Runtime.Serialization.OptionalFieldAttribute()]
        private int YPosField;
        
        [global::System.ComponentModel.BrowsableAttribute(false)]
        public System.Runtime.Serialization.ExtensionDataObject ExtensionData {
            get {
                return this.extensionDataField;
            }
            set {
                this.extensionDataField = value;
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public sbyte Object {
            get {
                return this.ObjectField;
            }
            set {
                if ((this.ObjectField.Equals(value) != true)) {
                    this.ObjectField = value;
                    this.RaisePropertyChanged("Object");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public int XPos {
            get {
                return this.XPosField;
            }
            set {
                if ((this.XPosField.Equals(value) != true)) {
                    this.XPosField = value;
                    this.RaisePropertyChanged("XPos");
                }
            }
        }
        
        [System.Runtime.Serialization.DataMemberAttribute()]
        public int YPos {
            get {
                return this.YPosField;
            }
            set {
                if ((this.YPosField.Equals(value) != true)) {
                    this.YPosField = value;
                    this.RaisePropertyChanged("YPos");
                }
            }
        }
        
        public event System.ComponentModel.PropertyChangedEventHandler PropertyChanged;
        
        protected void RaisePropertyChanged(string propertyName) {
            System.ComponentModel.PropertyChangedEventHandler propertyChanged = this.PropertyChanged;
            if ((propertyChanged != null)) {
                propertyChanged(this, new System.ComponentModel.PropertyChangedEventArgs(propertyName));
            }
        }
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    [System.ServiceModel.ServiceContractAttribute(ConfigurationName="AgentService.IAgentService", CallbackContract=typeof(Control.AgentService.IAgentServiceCallback))]
    public interface IAgentService {
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/IAgentService/RegisterClient", ReplyAction="http://tempuri.org/IAgentService/RegisterClientResponse")]
        void RegisterClient();
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    public interface IAgentServiceCallback {
        
        [System.ServiceModel.OperationContractAttribute(Action="http://tempuri.org/IAgentService/Inform", ReplyAction="http://tempuri.org/IAgentService/InformResponse")]
        void Inform(Control.AgentService.AgentPosition pos);
    }
    
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    public interface IAgentServiceChannel : Control.AgentService.IAgentService, System.ServiceModel.IClientChannel {
    }
    
    [System.Diagnostics.DebuggerStepThroughAttribute()]
    [System.CodeDom.Compiler.GeneratedCodeAttribute("System.ServiceModel", "3.0.0.0")]
    public partial class AgentServiceClient : System.ServiceModel.DuplexClientBase<Control.AgentService.IAgentService>, Control.AgentService.IAgentService {
        
        public AgentServiceClient(System.ServiceModel.InstanceContext callbackInstance) : 
                base(callbackInstance) {
        }
        
        public AgentServiceClient(System.ServiceModel.InstanceContext callbackInstance, string endpointConfigurationName) : 
                base(callbackInstance, endpointConfigurationName) {
        }
        
        public AgentServiceClient(System.ServiceModel.InstanceContext callbackInstance, string endpointConfigurationName, string remoteAddress) : 
                base(callbackInstance, endpointConfigurationName, remoteAddress) {
        }
        
        public AgentServiceClient(System.ServiceModel.InstanceContext callbackInstance, string endpointConfigurationName, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(callbackInstance, endpointConfigurationName, remoteAddress) {
        }
        
        public AgentServiceClient(System.ServiceModel.InstanceContext callbackInstance, System.ServiceModel.Channels.Binding binding, System.ServiceModel.EndpointAddress remoteAddress) : 
                base(callbackInstance, binding, remoteAddress) {
        }
        
        public void RegisterClient() {
            base.Channel.RegisterClient();
        }
    }
}