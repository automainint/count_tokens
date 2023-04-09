echo "Compile count_tokens.c"
gcc -O3 count_tokens.c -o count_tokens

rm temp.bin
i=0
while [ "$i" -lt 30 ]; do
  file=$(printf "%02d.jsonl" "$i")
  url=$(printf "https://the-eye.eu/public/AI/pile/train/%02d.jsonl.zst" "$i")
  printf "* %02d  Download ${url}..." "$i"
  wget "${url}"
  printf "* %02d  Extract archive" "$i"
  unzstd "${file}.zst" -o "${file}"
  rm "${file}.zst"
  printf "* %02d  Process data..." "$i"
  ./count_tokens "${file}" temp.bin result.csv
  rm "${file}"
  printf "* %02d  Done" "$i"
  i=$((i + 1))
done
echo "All done"
