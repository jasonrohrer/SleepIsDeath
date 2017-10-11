

if [[ "$#" < "2" ]] ; then
  echo "Usage:    makeList.sh listNumber listSourceFile.txt" 
  echo "Example:  makeList.sh 36 sidSeptember.txt"
  exit
fi

mkdir "fullSail$1"

cp "$2" "fullSail$1/list$1.txt"

dos2unix "fullSail$1/list$1.txt"


checkLineString="./checkDuplicates.pl "
filterLineString="./filterOutDuplicates.pl "

for (( c=$1; c>=1; c-- ))
do
  checkLineString="$checkLineString fullSail$c/list$c.txt"
  filterLineString="$filterLineString fullSail$c/list$c.txt"
done



echo -n $checkLineString > checkLine 
echo -n $filterLineString > filterLine 

sh filterLine > "fullSail$1/list$1_filtered.txt"


tag="fullSailClass$1"

echo

echo "Make sure that $tag exists as a tag on the server."

echo

echo -n "Then paste server hash of 'abc' here:  "

read hash

./bulk.pl "fullSail$1/list$1_filtered.txt" "$tag" abc "$hash"