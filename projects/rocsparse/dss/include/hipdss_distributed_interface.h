/*! \file */
/* ************************************************************************
 * Copyright (C) 2026 Advanced Micro Devices, Inc. All rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * ************************************************************************ */

#ifndef HIPDSS_DISTRIBUTED_INTERFACE_H
#define HIPDSS_DISTRIBUTED_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum hipdssOpType_t
{
    HIPDSS_SUM,
    HIPDSS_MAX,
    HIPDSS_MIN
} hipdssOpType_t;

typedef struct
{
    int (*hipdssCommRankDevice)(void* comm,
                                int*  rank);

    int (*hipdssCommSizeDevice)(void* comm,
                                int*  size);

    int (*hipdssSendDevice)(const void* buffer,
                            int         count,
                            hipDataType datatype,
                            int         dest,
                            int         tag,
                            void*       comm,
                            hipStream_t stream);

    int (*hipdssRecvDevice)(void*       buffer,
                            int         count,
                            hipDataType datatype,
                            int         root,
                            int         tag,
                            void*       comm,
                            hipStream_t stream);

    int (*hipdssBcastDevice)(void*       buffer,
                             int         count,
                             hipDataType datatype,
                             int         root,
                             void*       comm,
                             hipStream_t stream);

    int (*hipdssReduceDevice)(const void*    sendbuf,
                              void*          recvbuf,
                              int            count,
                              hipDataType    datatype,
                              hipdssOpType_t op,
                              int            root,
                              void*          comm,
                              hipStream_t    stream);

    int (*hipdssAllreduceDevice)(const void*    sendbuf,
                                 void*          recvbuf,
                                 int            count,
                                 hipDataType    datatype,
                                 hipdssOpType_t op,
                                 void*          comm,
                                 hipStream_t    stream);

    int (*hipdssScattervDevice)(const void* sendbuf,
                                const int*  sendcounts,
                                const int*  displs,
                                hipDataType sendtype,
                                void*       recvbuf,
                                int         recvcount,
                                hipDataType recvtype,
                                int         root,
                                void*       comm,
                                hipStream_t stream);

    int (*hipdssCommSplitDevice)(const void* comm,
                                 int         color,
                                 int         key,
                                 void*       newcomm);
    int (*hipdssCommFreeDevice)(void* comm);

    int (*hipdssCommRankHost)(void* comm,
                              int*  rank);

    int (*hipdssCommSizeHost)(void* comm,
                              int*  size);

    int (*hipdssSendHost)(const void* buffer,
                          int         count,
                          hipDataType datatype,
                          int         dest,
                          int         tag,
                          void*       comm,
                          hipStream_t stream);

    int (*hipdssRecvHost)(void*       buffer,
                          int         count,
                          hipDataType datatype,
                          int         source,
                          int         tag,
                          void*       comm,
                          hipStream_t stream);

    int (*hipdssBcastHost)(void*       buffer,
                           int         count,
                           hipDataType datatype,
                           int         root,
                           void*       comm,
                           hipStream_t stream);

    int (*hipdssReduceHost)(const void*    sendbuf,
                            void*          recvbuf,
                            int            count,
                            hipDataType    datatype,
                            hipdssOpType_t op,
                            int            root,
                            void*          comm,
                            hipStream_t    stream);

    int (*hipdssAllreduceHost)(const void*    sendbuf,
                               void*          recvbuf,
                               int            count,
                               hipDataType    datatype,
                               hipdssOpType_t op,
                               void*          comm,
                               hipStream_t    stream);

    int (*hipdssScattervHost)(const void* sendbuf,
                              const int*  sendcounts,
                              const int*  displs,
                              hipDataType sendtype,
                              void*       recvbuf,
                              int         recvcount,
                              hipDataType recvtype,
                              int         root,
                              void*       comm,
                              hipStream_t stream);

    int (*hipdssCommSplitHost)(const void* comm,
                               int         color,
                               int         key,
                               void*       newcomm);
    int (*hipdssCommFreeHost)(void* comm);

    int (*hipdssDistributedGetProperty)(libraryPropertyType propertyType,
                                        int*                value);

} hipdssDistributedInterface_t;

#ifdef __cplusplus
}
#endif

#endif
