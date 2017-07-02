pragma solidity ^0.4.0;

contract Ballot {

    Proposal[] arr;

    struct Proposal {
        uint size;
        string name;
        string hash; // md5&sha512
    }

    function pushed(uint size, string name, string hash)
    {
        arr.push(Proposal(size, name, hash));
    }
}
