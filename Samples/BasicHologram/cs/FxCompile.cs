//-----------------------------------------------------------------------
// <copyright file="FxCompile.cs" company="Microsoft">
// Copyright (C) Microsoft Corporation. All rights reserved.
// </copyright>
//-----------------------------------------------------------------------

using Microsoft.Build.Framework;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Microsoft.Build.Utilities;

namespace Microsoft.Build.Tasks
{
    /// <summary>
    /// Task to support Fxc.exe
    /// </summary>
    public class FxCompile : ToolTask
    {
        /// <summary>
        /// Constructor
        /// </summary>
        public FxCompile() 
        {
            // Because FxCop wants it this way.
        }

        #region Inputs

        /// <summary>
        /// Sources to be compiled.
        /// </summary>
        /// <remarks>Required for task to run.</remarks>
        [Required]
        public virtual ITaskItem[] Source
        {
            get { return (ITaskItem[])Bag["Sources"]; }
            set { Bag["Sources"] = value; }
        }

        /// <summary>
        /// Gets the collection of parameters used by the derived task class.
        /// </summary>
        /// <value>Parameter bag.</value>
        protected internal Hashtable Bag
        {
            get
            {
                return bag;
            }
        }

        private Hashtable bag = new Hashtable();
        
        /// <summary>
        /// ShaderType, requires ShaderModel
        /// Specifies the type of shader.  (/T [type]_[model])
        /// </summary>
        /// <remarks>Consider using one of these: "NotSet", "Effect", "Vertex", "Pixel", "Geometry", "Hull", "Domain", "Compute", or "Texture".</remarks>
        public virtual string ShaderType
        {
            get 
            { 
                return (string)Bag["ShaderType"]; 
            }

            set 
            {
                string result = String.Empty;
                switch (value.ToLowerInvariant())
                {
                    case "notset":
                        result = "";
                        break;
                    case "effect":
                        result = "/T fx";
                        break;
                    case "vertex":
                        result = "/T vs";
                        break;
                    case "pixel":
                        result = "/T ps";
                        break;
                    case "geometry":
                        result = "/T gs";
                        break;
                    case "hull":
                        result = "/T hs";
                        break;
                    case "domain":
                        result = "/T ds";
                        break;
                    case "compute":
                        result = "/T cs";
                        break;
                    case "texture":
                        result = "/T tx";
                        break;
                    default:
                        throw new ArgumentException("ShaderType of " + value + @" is an invalid.  Consider using one of these: ""NotSet"", ""Effect"", ""Vertex"", ""Pixel"", ""Geometry"", ""Hull"", ""Domain"", ""Compute"", or ""Texture"".");
                }

                Bag["ShaderType"] = result;
            }
        }

        /// <summary>
        /// ShaderModel, requires ShaderType
        /// Specifies the shader model. Some shader types can only be used with recent shader models (/T [type]_[model])
        /// </summary>
        public virtual string ShaderModel
        {
            get { return (string)Bag["ShaderModel"]; }
            set { Bag["ShaderModel"] = value; }
        }

        /// <summary>
        /// AssemblerOutput, requires AssemblerOutputFile
        /// Specifies the contents of assembly language output file. (/Fc, /Fx)
        /// </summary>
        /// <remarks>Consider using one of these: "Assembly Code" or "Assembly Code and Hex".</remarks>
        public virtual string AssemblerOutput
        {
            get 
            {
                return (string)Bag["AssemblerOutput"]; 
            }

            set 
            {
                string result = String.Empty;
                switch (value.ToLowerInvariant())
                {
                    case "Assembly Code":
                        result = "/Fc";
                        break;
                    case "Assembly Code and Hex":
                        result = "/Fx";
                        break;
                    default:
                        throw new ArgumentException("AssemblerOutput of " + value + @" is an invalid.  Consider using one of these: ""Assembly Code"" or ""Assembly Code and Hex"".");
                }

                Bag["AssemblerOutput"] = value; 
            }
        }

        /// <summary>
        /// AssemblerOutputFile, requires AssemblerOutput
        /// Specifies file name for assembly code listing file
        /// </summary>
        public virtual string AssemblerOutputFile
        {
            get { return (string)Bag["AssemblerOutputFile"]; }
            set { Bag["AssemblerOutputFile"] = value; }
        }

        /// <summary>
        /// Specifies a name for the variable name in the header file (/Vn [name])
        /// </summary>
        public virtual string VariableName
        {
            get { return (string)Bag["VariableName"]; }
            set { Bag["VariableName"] = value; }
        }

        /// <summary>
        /// Specifies a name for header file containing object code. (/Fh [name])
        /// </summary>
        public virtual string HeaderFileOutput
        {
            get { return (string)Bag["HeaderFileOutput"]; }
            set { Bag["HeaderFileOutput"] = value; }
        }
        
        /// <summary>
        /// Specifies a name for object file. (/Fo [name])
        /// </summary>
        public virtual string ObjectFileOutput
        {
            get { return (string)Bag["ObjectFileOutput"]; }
            set { Bag["ObjectFileOutput"] = value; }
        }

        /// <summary>
        /// Defines preprocessing symbols for your source file.
        /// </summary>
        public virtual string[] PreprocessorDefinitions
        {
            get { return (string[])Bag["PreprocessorDefinitions"]; }
            set { Bag["PreprocessorDefinitions"] = value; }
        }

        /// <summary>
        /// Specifies one or more directories to add to the include path; separate with semi-colons if more than one. (/I[path])
        /// </summary>
        public virtual string[] AdditionalIncludeDirectories
        {
            get { return (string[])Bag["AdditionalIncludeDirectories"]; }
            set { Bag["AdditionalIncludeDirectories"] = value; }
        }

        /// <summary>
        /// Suppresses the display of the startup banner and information message. (/nologo)
        /// </summary>
        public virtual bool SuppressStartupBanner
        {
            get { return GetBoolParameterWithDefault("SuppressStartupBanner", false); }
            set { Bag["SuppressStartupBanner"] = value; }
        }

        /// <summary>
        /// Specifies the name of the entry point for the shader (/E[name])
        /// </summary>
        public virtual string EntryPointName
        {
            get { return (string)Bag["EntryPointName"]; }
            set { Bag["EntryPointName"] = value; }
        }

        /// <summary>
        /// Treats all compiler warnings as errors. For a new project, it may be best to use /WX in all compilations; resolving all warnings will ensure the fewest possible hard-to-find code defects.
        /// </summary>
        public virtual bool TreatWarningAsError
        {
            get { return GetBoolParameterWithDefault("TreatWarningAsError", false); }
            set { Bag["TreatWarningAsError"] = value; }
        }

        /// <summary>
        /// Disable optimizations. /Od implies /Gfp though output may not be identical to /Od /Gfp.
        /// </summary>
        public virtual bool DisableOptimizations
        {
            get { return GetBoolParameterWithDefault("DisableOptimizations", false); }
            set { Bag["DisableOptimizations"] = value; }
        }

        /// <summary>
        /// Enable debugging information.
        /// </summary>
        public virtual bool EnableDebuggingInformation
        {
            get { return GetBoolParameterWithDefault("EnableDebuggingInformation", false); }
            set { Bag["EnableDebuggingInformation"] = value; }
        }

        /// <summary>
        /// Path to Windows SDK
        /// </summary>
        public string SdkToolsPath
        {
            get { return (string)Bag["SdkToolsPath"]; }
            set { Bag["SdkToolsPath"] = value; }
        }

        /// <summary>
        /// Name to Fxc.exe
        /// </summary>
        protected override string ToolName
        {
            get { return "Fxc.exe"; }
        }

        #endregion

        /// <summary>
        /// Returns a string with those switches and other information that can't go into a response file and
        /// must go directly onto the command line.
        /// Called after ValidateParameters and SkipTaskExecution
        /// </summary>
        /// <returns></returns>
        override protected string GenerateCommandLineCommands()
        {
            CommandLineBuilderExtension commandLineBuilder = new CommandLineBuilderExtension();
            AddCommandLineCommands(commandLineBuilder);
            return commandLineBuilder.ToString();
        }
        
        /// <summary>
        /// Returns the command line switch used by the tool executable to specify the response file
        /// Will only be called if the task returned a non empty string from GetResponseFileCommands
        /// Called after ValidateParameters, SkipTaskExecution and GetResponseFileCommands
        /// </summary>
        /// <param name="responseFilePath">full path to the temporarily created response file</param>
        /// <returns></returns>
        override protected string GenerateResponseFileCommands()
        {                         
            CommandLineBuilderExtension commandLineBuilder = new CommandLineBuilderExtension();
            AddResponseFileCommands(commandLineBuilder);
            return commandLineBuilder.ToString();
        }
        
        /// <summary>
        /// Fills the provided CommandLineBuilderExtension with those switches and other information that can go into a response file.
        /// </summary>
        /// <param name="commandLine"></param>
        protected internal virtual void AddResponseFileCommands(CommandLineBuilderExtension commandLine)
        {
        }
        
        /// <summary>
        /// Add Command Line Commands
        /// </summary>
        /// <param name="commandLine">CommandLineBuilderExtension</param>
        protected internal void AddCommandLineCommands(CommandLineBuilderExtension commandLine)
        {
            //// Order of these affect the order of the command line

            commandLine.AppendSwitchIfNotNull("/I ", AdditionalIncludeDirectories, "");
            commandLine.AppendSwitch(SuppressStartupBanner ? "/nologo" : String.Empty);
            commandLine.AppendSwitchIfNotNull("/E", EntryPointName);
            commandLine.AppendSwitch(TreatWarningAsError ? "/WX" : String.Empty);

            // Switch cannot be null
            if (ShaderType != null && ShaderModel != null)
            {
                // shader Model and Type are one switch
                commandLine.AppendSwitch(ShaderType + "_" + ShaderModel);
            }

            commandLine.AppendSwitchIfNotNull("/D ", PreprocessorDefinitions, "");
            commandLine.AppendSwitchIfNotNull("/Fh ", HeaderFileOutput);
            commandLine.AppendSwitchIfNotNull("/Fo ", ObjectFileOutput);

            // Switch cannot be null
            if (AssemblerOutput != null)
            {
                commandLine.AppendSwitchIfNotNull(AssemblerOutput, AssemblerOutputFile);
            }
            
            commandLine.AppendSwitchIfNotNull("/Vn ", VariableName);
            commandLine.AppendSwitch(DisableOptimizations ? "/Od" : String.Empty);
            commandLine.AppendSwitch(EnableDebuggingInformation ? "/Zi" : String.Empty);
            
            commandLine.AppendSwitchIfNotNull("", Source, " ");
        }

        /// <summary>
        /// Fullpath to the fxc.exe
        /// </summary>
        /// <returns>Fullpath to fxc.exe, if found.  Otherwise empty or null.</returns>
        protected override string GenerateFullPathToTool()
        {
            return System.IO.Path.Combine(SdkToolsPath, ToolName);
        }
        
        /// <summary>
        /// Get a bool parameter and return a default if its not present
        /// in the hash table.
        /// </summary>
        /// <param name="parameters"></param>
        /// <param name="parameterName"></param>
        /// <param name="defaultValue"></param>
        /// <returns></returns>
        /// <owner>JomoF</owner>
        protected internal bool GetBoolParameterWithDefault(string parameterName, bool defaultValue)
        {
            object obj = bag[parameterName];
            return (obj == null) ? defaultValue : (bool) obj;
        }
    }
}
