//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************


using System;
using Windows.Storage.Streams;
using System.Runtime.InteropServices.WindowsRuntime;

public class TlvEntry
{
    public TlvEntry(ushort tag, object value)
    {
        Tag = tag;
        Value = value;
    }

    ushort Tag
    {
        get;
        set;
    }

    byte TagLength
    {
        get
        {
            if ((Tag & 0xFF00) != 0)
            {
                return 2;
            }
            else
            {
                return 1;
            }
        }
    }

    object Value
    {
        get;
        set;
    }

    int ValueLength
    {
        get
        {
            if (Value is string)
            {
                return ((string)Value).Length;
            }
            else if (Value is IBuffer)
            {
                return (int)((IBuffer)Value).Length;
            }
            else if(Value is byte[])
            {
                return ((byte[])Value).Length;
            }
            else if(Value is TlvEntry)
            {
                return ((TlvEntry)Value).Length;
            }
            else if (Value is TlvEntry[])
            {
                int ret = 0;
                foreach(var entry in (TlvEntry[])Value)
                {
                    ret += entry.Length;
                }
                return ret;
            }
            throw new InvalidOperationException();
        }
    }

    int Length
    {
        get
        {
            return TagLength 
                + (ValueLength > 0x7F ? 2 : 1)
                + ValueLength;
        }
    }

    public byte[] GetData(ushort? statusWord = null)
    {
        int i = 0;
        var ret = new byte[this.Length + (statusWord == null ? 0 : 2)];
        
        if (TagLength == 2)
        {
            ret[i++] = (byte)((Tag & 0xFF00) >> 8);
            ret[i++] = (byte)(Tag & 0xFF);
        }
        else
        {
            ret[i++] = (byte)Tag;
        }

        if (ValueLength > 0x7F)
        {
            if (ValueLength > 0xFF)
            {
                throw new NotImplementedException();
            }
            ret[i++] = 0x81;
        }
        ret[i++] = (byte)ValueLength;

        if (Value is string)
        {
            Value = System.Text.UTF8Encoding.UTF8.GetBytes((string)Value);
        }
        else if (Value is IBuffer)
        {
            Value = ((IBuffer)Value).ToArray();
        }


        if (Value is byte[])
        {
            Array.Copy((byte[])Value, 0, ret, i, ((byte[])Value).Length);
            i += ((byte[])Value).Length;
        }
        else if (Value is TlvEntry)
        {
            Array.Copy(((TlvEntry)Value).GetData(), 0, ret, i, ((TlvEntry)Value).Length);
            i += ((TlvEntry)Value).Length;
        }
        else if (Value is TlvEntry[])
        {
            foreach (var entry in (TlvEntry[])Value)
            {
                Array.Copy(entry.GetData(), 0, ret, i, entry.Length);
                i += entry.Length;
            }
        }
        else
        {
            throw new InvalidOperationException();
        }

        if (statusWord != null)
        {
            ret[i++] = (byte)((statusWord & 0xFF00) >> 8);
            ret[i++] = (byte)(statusWord & 0xFF);
        }
        return ret;
    }
}