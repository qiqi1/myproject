/*++

Copyright (c) Microsoft Corporation. All rights reserved

Abstract:

   Stream Edit Callout Driver Sample.

   This sample demonstrates finding and replacing a string pattern from a
   live TCP stream via the WFP stream API.

--*/

#ifndef _STREAM_CALLOUT_H
#define _STREAM_CALLOUT_H

extern PMDL gStringToReplaceMdl;
extern HANDLE gInjectionHandle;
extern NDIS_HANDLE gNetBufferListPool;
extern STREAM_EDITOR gStreamEditor;

// 
// Configurable parameters
//

extern USHORT  configInspectionPort;
extern BOOLEAN configInspectionOutbound; 


extern CHAR configStringToFind[];
extern CHAR configStringToReplace[];

#pragma warning(push)
#pragma warning(disable:4201)       // unnamed struct/union

typedef struct STREAM_EDITOR_
{
   BOOLEAN editInline;

   union
   {
      INLINE_EDIT_STATE inlineEditState;
   };

   PVOID scratchBuffer;
   SIZE_T bufferSize;
   SIZE_T dataOffset;
   SIZE_T dataLength;

}STREAM_EDITOR;

#pragma warning(pop)

BOOLEAN
StreamCopyDataForInspection(
   IN STREAM_EDITOR* streamEditor,
   IN const FWPS_STREAM_DATA* streamData
   );

extern
NTSYSAPI
NTSTATUS
NTAPI
RtlUnicodeToMultiByteN(
    __out_bcount_part(MaxBytesInMultiByteString, *BytesInMultiByteString) PCHAR MultiByteString,
    __in ULONG MaxBytesInMultiByteString,
    __out_opt PULONG BytesInMultiByteString,
    __in_bcount(BytesInUnicodeString) PWCH UnicodeString,
    __in ULONG BytesInUnicodeString
    );

#endif // _STREAM_CALLOUT_H

