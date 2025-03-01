# Huffman coding

c++ implementation of [Huffman coding challenge](https://codingchallenges.fyi/challenges/challenge-huffman).

## Usage

## About Huffman coding

This tool is used to compress text files (in .txt format only) into .hz archive. First frequency of each character in input file if calculated. From these frequencies a Huffman tree is created. And finally a code table is created from the tree. This table is then used to encode data.

Data contained in the nodes of Huffman tree are simply a character and its frequency for leaf nodes. Or just frequency for internal nodes. The structure of the tree defines final codes for characters. *0* means a move left, *1* a move right. In the encoding process, the tree is used to generate code table. But in the decoding process, the tree is used directly to decode bits using the previously mentioned moves.

### Huffman tree creation

1. Create a single node tree for every character.
2. Put them into min priority queue, sorted by frequency.
3. Remove the first two nodes (those with the lowest frequency). Join them to a new tree where these two nodes are children (the orientation doesn't matter). The root's frequency is equal to the sum of both frequencies.
4. Put this new tree back into the priority queue. (Note, that the priority queue is always ordered.)
5. Repeat until only one tree is left in the priority queue. This is the Huffman tree.

## Sources

- [Wiki](https://en.wikipedia.org/wiki/Huffman_coding)
- [OpenDSA](https://opendsa-server.cs.vt.edu/ODSA/Books/CS3/html/Huffman.html)
- [Huffman tree generator](https://www.csfieldguide.org.nz/en/interactives/huffman-tree/)

## Notes

We are trying to encode UTF-8 characters. But maybe we could also encode just plain bytes.
