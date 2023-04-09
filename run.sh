#gcc count_tokens.c -o count_tokens

i=0
while [ "$i" -lt 30 ]; do
  url=$(printf "https://the-eye.eu/public/AI/pile/train/%02d.jsonl.zst" "$i")
  echo "Counting ${url}..."

  i=$((i + 1))
done
