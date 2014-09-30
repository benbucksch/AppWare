homedir=$pwd
logFile="$homedir\\dofiles.log"
logFile=$(fullpath $logFile)
tmpFile="$homedir\\tmp_zzz.xxx"
tmpFile=$(fullpath $tmpFile)

echo "\t** STARTING **">$logFile

for file in $(whereis $homeDir *.h *.cpp *.hpp *.i)
  do
  echo "******* $file *******" 
  basefile=`basename $file`

  sed -f /appwre32/scripts/autorepl.sed $file > $tmpFile
  cmp -s $file $tmpFile
  if [ "$?" != "0" ]
    then
    echo "\t$file"
    dir=$(dirname $file)
    dir=$(fullpath $dir)
    ssdir=$(awkw -f /appwre32/scripts/dirtoss.awk $dir $1) 
    echo "\tss cd $ssdir"
    ss cd $ssdir
    echo "\tcd $dir"
    cd $dir
    echo "\tss check $basefile"
    ss check $basefile
    echo "\tmv $tmpFile $basefile"
    mv -f $tmpFile $basefile
    echo "\tss update $basefile -C@/appwre32/scripts/comment.txt"
    ss update $basefile -C@/appwre32/scripts/comment.txt
    echo "\t$basefile...done\n"
    else
    echo "(\t$file doesn't need to be changed)"
    fi 
  
  echo "\trm -f $tmpFile"
  rm -f $tmpFile
  
  done >> $logFile
