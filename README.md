ΦΟΙΤΗΤΕΣ ΠΟΥ ΥΛΟΠΟΙΗΣΑΝ ΤΗΝ ΕΡΓΑΣΙΑ
-ΡΟΥΣΣΟΣ ΒΑΣΙΛΕΙΟΣ 1115200700224
-ΔΗΜΑΚΑΚΟΣ ΠΑΝΑΓΙΩΤΗΣ 1115202000268
-ΟΡΦΑΝΑΚΟΥ ΦΟΙΒΗ 1115202000156

ΠΑΡΑΤΗΡΗΣΕΙΣ

- Αλλαξαμε το μεγεθος των strings του record, γιατι καποια επωνυμα και πολεις δε χωρουσαν (το '\0') και υπηρχαν errors καποιες φορες κατα
  την εκτελεση. Μετα την αλλαγη δεν παρατηρησαμε ξανα καποιο error.

- Chat Gpt link: https://chat.openai.com/share/8195b1c3-8647-46d2-b9af-d3814b9c1ecc

- chunk:

    CHUNK_CreateIterator: No changes

    CHUNK_GetNext: Implemented record counting logic, will special handle on last block

    CHUNK_GetIthRecordInChunk: Used HP_Unpin and improved CalculateBlockIdAndCursor method for last block

    CHUNK_UpdateIthRecord: Used HP_Unpin and improved CalculateBlockIdAndCursor method for last block

    CHUNK_Print: Improved check value use

    CHUNK_CreateRecordIterator: Made it almost from start, chat confused parameter type

    CHUNK_GetNextRecord: Improved code format and logic, used HP_Unpin

- sort:
   
   shouldSwap: Done alone, was super easy

   sort_FileInChunks: Used some extra prints, improved check

   sort_Chunk: Error reporting

- merge:

    merge: Changed the implementation from chat, to work properly and be more clean. It could not get the whole picture
           so we took over and made it work