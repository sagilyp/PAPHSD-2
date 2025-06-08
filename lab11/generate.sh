#!/bin/bash

DESTDIR="codebase_10000"
mkdir -p "$DESTDIR"
for i in $(seq -w 1 10000); do
    fname="$DESTDIR/file${i}.txt"
    # Записываем две строки-заглушки:
    echo "// file #${i} — placeholder" > "$fname"
    echo "int dummy_${i} = ${i};" >> "$fname"
done
echo "Готово: 10 000 файлов в папке $DESTDIR."
