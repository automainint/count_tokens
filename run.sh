echo "Compile count_tokens.c"
gcc -O3 count_tokens.c -o count_tokens

rm temp.bin
i=0
while [ "$i" -lt 30 ]; do
  file=$(printf "%02d.jsonl" "$i")
  url=$(printf "https://the-eye.eu/public/AI/pile/train/%02d.jsonl.zst" "$i")
  echo "$i Download ${url}"
  wget "${url}"
  echo "$i Extract archive"
  unstzip "${file}.zst" "${file}"
  rm "${file}.zst"
  echo "$i Process data..."
  ./count_tokens "${file}" temp.bin result.csv
  rm "${file}"
  echo "$i Done"
  i=$((i + 1))
done
echo "All done"
