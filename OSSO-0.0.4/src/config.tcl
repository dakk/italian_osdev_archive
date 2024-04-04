wm title . "OSSO Configuration Program"

proc read_config { filename sectionname parent } {
    set file [open $filename r]
    while { [gets $file line ] >=0 } {
	if [regexp {( *\#)(.*)} $line foo ok rest] {
#	    puts "comment : $ok - $rest"
	} elseif [regexp {([-_0-9A-Za-z]+) (boolean|define|string|value) ([0-9A-Za-z_]+) ([0-9A-Za-z_]+) (.+)} $line foo name vartype varname varvalue vartext] {
	    if {$vartype == "boolean" || $vartype == "define"} then {
		set ff [join [concat $sectionname $varname] ""]
		set cmd "global $ff ; set $ff $varvalue;checkbutton .$parent.$varname-chk -variable $ff -text \"$vartext\";pack .$parent.$varname-chk"
		eval $cmd
		#puts "$ff"
	    }
	    if {$vartype == "string" || $vartype == "value"} then {
		set cmd "frame .$parent.$varname-frame ; pack .$parent.$varname-frame -fill x"
		eval $cmd
		set cmd "label .$parent.$varname-frame.$varname-lab -text \"$vartext\";entry .$parent.$varname-frame.$varname; .$parent.$varname-frame.$varname insert 0 \"$varvalue\""
		eval $cmd
		set cmd "pack .$parent.$varname-frame.$varname-lab -side left ; pack .$parent.$varname-frame.$varname -side right"
		eval $cmd
	    }
	}
	if [regexp {([-_0-9A-Za-z]+) (makevar|makestring) ([0-9A-Za-z_]+) ([0-9A-Za-z_:/.]+) ([0-9A-Za-z_]+) (.+)} $line foo name vartype varname varvalue makevar vartext] {
	    if {$vartype == "makevar"} then {
		set ff [join [concat $sectionname $varname] ""]
		set cmd "global $ff ; set $ff $varvalue;checkbutton .$parent.$varname-chk -variable $ff -text \"$vartext\";pack .$parent.$varname-chk"
		eval $cmd
	    }
	    if {$vartype == "makestring"} then {
		set cmd "frame .$parent.$varname-frame ; pack .$parent.$varname-frame -fill x"
		eval $cmd
		set cmd "label .$parent.$varname-frame.$varname-lab -text \"$vartext\";entry .$parent.$varname-frame.$varname; .$parent.$varname-frame.$varname insert 0 \"$varvalue\""
		eval $cmd
		set cmd "pack .$parent.$varname-frame.$varname-lab -side left ; pack .$parent.$varname-frame.$varname -side right"
		eval $cmd
	    }
	}

    }
    close $file
}

proc read_sections { filename } {
    set file [open $filename r]
    while { [gets $file line ] >=0 } {
	if [regexp {( *\#)(.*)} $line foo ok rest] {
#	    puts "comment : $ok - $rest"
	} elseif [regexp {([-_0-9A-Za-z]+),([0-9A-Za-z_ ]+),([0-9A-Za-z_ ]+),([0-9A-Za-z_./_]+)} $line foo sectionfile sectionname title configfile] {	
	    .notebook add "nb$sectionname" -label $title	
	    read_config $sectionfile $sectionname "notebook.nbframe.nb$sectionname"
#	    puts $sectionfile
#	    puts $title
#	    puts $configfile
	} else {
	    puts "Warning : Ignored Line : $line"
	}
    }
    pack .notebook
    close $file
}

proc write_header_c { file id } {
    puts -nonewline $file "#ifndef _$id"
    puts $file "_CONF_FILE_H_"
    puts -nonewline $file "#define _$id"
    puts $file "_CONF_FILE_H_"
    puts $file ""
}

proc write_footer_c { file } {
    puts $file ""
    puts $file "#endif"
}

proc write_define { file name } {
    puts -nonewline $file "#define "
    puts -nonewline $file $name
    puts -nonewline $file " "   
}

proc write_config { filename } {
    set file [open $filename r]
    while { [gets $file line ] >=0 } {
	if [regexp {( *\#)(.*)} $line foo ok rest] {
	    
	} elseif [regexp {([-_0-9A-Za-z]+),([0-9A-Za-z_ ]+),([0-9A-Za-z_ ]+),([0-9A-Za-z_./]+)} $line foo sectionfile sectionname title configfile] {
	    write_config_file $sectionfile $sectionname "notebook.nbframe.nb$sectionname" $configfile
	}
    }
    pack .notebook
    close $file
}

proc save_config { filename savefilename} {
    set file [open $filename r]
    set savefile [open $savefilename w]
    while { [gets $file line ] >=0 } {
	if [regexp {( *\#)(.*)} $line foo ok rest] {
	    
	} elseif [regexp {([-_0-9A-Za-z]+),([0-9A-Za-z_ ]+),([0-9A-Za-z_ ]+),([0-9A-Za-z_./]+)} $line foo sectionfile sectionname title configfile] {
	    save_config_file $sectionfile $sectionname "notebook.nbframe.nb$sectionname" $savefile
	}
    }
    close $file
    close $savefile
}

proc write_config_file { filename sectionname parent outputfile } {
    set file [open $filename r]
    set configfile [open $outputfile w]
#    puts "opening $filename ..."
#    write_header $configfile [string toupper $sectionname]
    while { [gets $file line ] >=0 } {
	if [regexp {(write) (header|footer|string) (.+)} $line foo op obj data] {
	    if {$obj == "header"} then {
		write_header_c $configfile [string toupper $sectionname]
	    } 
	    if {$obj == "footer"} then {
		write_footer_c $configfile
	    }
	    if {$obj == "string"} then {
		puts $configfile $data
	    }
	}
	if [regexp {([-_0-9A-Za-z]+) (define|value|boolean|string) ([0-9A-Za-z_]+) ([0-9A-Za-z_]+) (.+)} $line foo name vartype varname varvalue vartext] {
	    if {$vartype == "boolean"} then {
		set ff [join [concat $sectionname $varname] ""]
		write_define $configfile $name
		set cmd "global $ff"
		eval $cmd
		set cmd "puts $configfile \$$ff"
		eval $cmd
	    }
	    if {$vartype == "define"} then {
		set ff [join [concat $sectionname $varname] ""]
		set cmd "global $ff"
		eval $cmd
		set cmd "set s \$$ff"
		eval $cmd
#		puts $s
		if {$s == 1} {
		    write_define $configfile $name
		    puts $configfile ""
		}
	    }

	    if {$vartype == "string"} then {
		write_define $configfile $name
		set cmd "set a [.$parent.$varname-frame.$varname get]"
		eval $cmd
		puts -nonewline $configfile "\""
		puts -nonewline $configfile $a
		puts $configfile "\""
	    }
	    if {$vartype == "value"} then {
		write_define $configfile $name
		set cmd "set a [.$parent.$varname-frame.$varname get]"
		eval $cmd
		puts $configfile $a
	    }
#	puts stdout $line
#	puts $name
#	puts stdout $vartype
#	puts stdout $varname
#	puts stdout $varvalue
#	puts stdout $vartext
	}
	if [regexp {([-_0-9A-Za-z]+) (makevar|makestring) ([0-9A-Za-z_]+) ([0-9A-Za-z_:/.]+) ([0-9A-Za-z_]+) (.+)} $line foo name vartype varname varvalue makevar vartext] {
	    if {$vartype == "makevar"} then {
		set ff [join [concat $sectionname $varname] ""]
		puts -nonewline $configfile $makevar
		puts -nonewline $configfile " += "
		set cmd "global $ff"
		eval $cmd
		set cmd "if {\$$ff==1} then {puts $configfile $name} else {puts $configfile \"\"}"
		eval $cmd
	    }
	    if {$vartype == "makestring"} then {
		set cmd "set a [.$parent.$varname-frame.$varname get]"
		eval $cmd
		puts $configfile "export $makevar = $a"
	    }
	    

	}
    }
#    puts "closing $filename ..."
    close $file
#    write_footer $configfile
    close $configfile
    puts "Finished creating $outputfile"
}

proc save_config_file { filename sectionname parent savefile } {
    set file [open $filename r]
    while { [gets $file line ] >=0 } {
	if [regexp {( *\#)(.*)} $line foo ok rest] {
#	    puts "comment : $ok - $rest"
	} elseif [regexp {([-_0-9A-Za-z]+) (boolean|define|string|value) ([0-9A-Za-z_]+) ([0-9A-Za-z_]+) (.+)} $line foo name vartype varname varvalue vartext] {
	    if {$vartype == "boolean" || $vartype == "define"} then {
		set ff [join [concat $sectionname $varname] ""]
		set cmd "global $ff;set tt \$$ff"
		eval $cmd
		set cmd "global $ff;set $ff $tt"
		puts $savefile $cmd
	    }
	    if {$vartype == "string" || $vartype == "value"} then {

		set cmd "set a [.$parent.$varname-frame.$varname get]"
		eval $cmd
		set cmd "set b .$parent.$varname-frame.$varname"
		eval $cmd

		puts $savefile "$b delete 0 \[string length \[$b get\]\]"
		puts -nonewline $savefile "$b insert 0 \""
		puts -nonewline $savefile $a
		puts $savefile "\""
	    }
	}
	if [regexp {([-_0-9A-Za-z]+) (makevar|makestring) ([0-9A-Za-z_]+) ([0-9A-Za-z_:/.]+) ([0-9A-Za-z_]+) (.+)} $line foo name vartype varname varvalue makevar vartext] {
	    if {$vartype == "makevar"} then {
		set ff [join [concat $sectionname $varname] ""]
		set cmd "global $ff;set tt \$$ff"
		eval $cmd
		set cmd "global $ff;set $ff $tt"
		puts $savefile $cmd
	    }
	    if {$vartype == "makestring"} then {

		set cmd "set a [.$parent.$varname-frame.$varname get]"
		eval $cmd
		set cmd "set b .$parent.$varname-frame.$varname"
		eval $cmd

		puts $savefile "$b delete 0 \[string length \[$b get\]\]"
		puts -nonewline $savefile "$b insert 0 \""
		puts -nonewline $savefile $a
		puts $savefile "\""
	    }

	}
    }
    close $file
}


#***************** MAIN ******************

puts "Launching OSSO Configurator..."
package require Tix

label .msg -text "Welcome to the first ever OSSO Configuration Program!"
pack .msg

tixNoteBook .notebook
read_sections "OSSO-config-sections"
#read_config OSSO-config-parms

frame .main-buttons; 
frame .main-buttons2;
frame .main-buttons3;

pack .main-buttons -fill x

button .main-buttons.generate-config -text "Generate config.h" -command {
    write_config OSSO-config-sections
}

button .main-buttons.make-clean -text "Make Clean" -command {
  exec -- make clean &
}

button .main-buttons.make-dep -text "Make Dep" -command {
  exec -- make dep &
}

button .main-buttons2.make-disk -text "Make OSSO boot disk" -command {
  exec -- make disk &
}


button .main-buttons2.make-floppy -text "Make OSSO boot floppy" -command {
  exec -- make floppy &
}

button .main-buttons3.load -text "Load Config" -command {
    set loadfile [open "OSSO.CONFIG" r]
    while { [gets $loadfile line ] >=0 } {
	eval $line
    }
    close $loadfile
}

button .main-buttons3.save -text "Save Config" -command {
    save_config "OSSO-config-sections" "OSSO.CONFIG"
}

button .main-buttons3.quits -text "Exit" -command {
    puts "Done."
    destroy .
}

pack .main-buttons2 -fill x
pack .main-buttons3 -fill x

pack .main-buttons.generate-config .main-buttons.make-clean .main-buttons.make-dep -side left -expand 1 -fill x
pack .main-buttons2.make-disk .main-buttons2.make-floppy -expand 1 -fill x -side left
pack .main-buttons3.load .main-buttons3.save .main-buttons3.quits  -expand 1 -fill x -side left




