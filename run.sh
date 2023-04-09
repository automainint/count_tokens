echo "Compile count_tokens.c"
gcc -O3 count_tokens.c -o count_tokens

rm -f temp.bin
i=0
while [ "$i" -lt 30 ]; do
  file=$(printf "%02d.jsonl" "$i")
  url=$(printf "https://the-eye.eu/public/AI/pile/train/%02d.jsonl.zst" "$i")
  printf "* %02d  Download ${url}\n" "$i"
  rm -f "${file}.zst"
  wget -q "${url}"
  printf "* %02d  Extract archive\n" "$i"
  unzstd --rm "${file}.zst" -o "${file}"
  printf "* %02d  Process data\n" "$i"
  ./count_tokens "${file}" temp.bin result.csv
  rm "${file}"
  printf "* %02d  Done\n" "$i"
  i=$((i + 1))
done
echo "All done"
