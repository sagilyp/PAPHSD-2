cd ./hg
# Меняем содержимое чётных файлов: добавляем строку "// changed"
for i in $(seq -w 2 2 10000); do
  echo "// changed at $(date +%s)" >> "file${i}.txt"
done
