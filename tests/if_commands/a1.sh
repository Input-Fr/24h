echo world
if true; then
ls
fi


if false; then
	echo toto;
else if false:; then
	echo check;
else if true; then
	echo not_check;	
else 
	echo test_check;
fi

if false; then
        echo toto;
else if false:; then
        if true; then
                echo check;
        fi
else if true; then
        echo not_check;
else
        echo test_check;
fi
