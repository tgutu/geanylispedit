if [ -e geanylispedit.so ];
	then
		if [ -d /usr/lib/geany ];
			then
				sudo cp geanylispedit.so /usr/lib/geany/geanylispedit.so
				echo 'The plugin geanylispedit.so was installed successfully!'
			else
				echo 'ERROR: The directory /usr/lib/geany does not exist. Geany is probably not installed, if it is please copy geanylispedit.so manually.'
		fi
	else
		echo 'ERROR: The file geanylispedit.so does not exist!!!'
fi
