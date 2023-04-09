# count\_tokens
Count tokens in The Pile [training data](https://the-eye.eu/public/AI/pile/train/).
- `https://the-eye.eu/public/AI/pile/train/00.jsonl.zst`
- ...
- `https://the-eye.eu/public/AI/pile/train/29.jsonl.zst`

##  Usage
- Download and decompress `jsonl` files.
- Compile `count_tokens.c`
- Run `count_tokens data.jsonl temp.bin [output.csv]`

Output `csv` file format:
```
<token_index>,<quantity>
<token_index>,<quantity>
```

All zero-quantity tokens will be at the end.

##  Quink run
On Linux, just execute `run.sh` script. It will do all the work automatically.
