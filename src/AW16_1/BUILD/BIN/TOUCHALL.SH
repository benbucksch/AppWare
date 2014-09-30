echo "Touching all files in $PWD"

for i in *
	do
	if [ -d $i ]
	then
		echo $0
		echo $i
		cd ./$i
		$0 $*
		cd ..
	elif [ "$i" != "BWCC.DLL" ]
	then
		echo touch -f -d $1 -t $2 $i
		touch -f -d $1 -t $2 $i
	fi
	done
