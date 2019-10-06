#!/bin/sh

   projdir=$( cat .le/.config | grep -o '^projdir .*' | cut -c "9-" )
   ignore=$( cat .le/.config | grep -E '^ignore .*' | cut -c "8-" )
if [ -z $1 ] ; then    #ak nieje zadany parameter

   for file in * ; do
      diff -s "$projdir/$file" "$file"  &>/dev/null #2 subory rovnake 
      if [ $? == 0 ] ; then
      echo ".: $file"  
      fi
      
      if [ -e "$file" ] && [ ! -e "$projdir/$file" ] ; then 
        echo "D: $file"     
      fi
     diff -u "$projdir/$file" "$file" 2>/dev/null 
   done
   for fileC in $projdir/* ; do
      filePOM=$( basename $fileC )
      if [ -e "$fileC" ] && [ ! -e $filePOM  ] ; then
        echo "C: $filePOM"                  
      fi 
   done
else
   for file2 in "$@" ; do
      diff -s "$projdir/$file2" "$file2" &>/dev/null
      if [ $? == 0 ] ; then
      echo .: $file2  
      fi

      if [ -e "$file2" ] && [ ! -e "$projdir$file2" ] ; then
        echo "D: $file2"
      fi

     diff -u "$projdir/$file2" "$file2" 2>/dev/null
   done

   for fileC2 in "$@" ; do
      if [ -e "$projdir/$fileC2" ] && [ ! -e $fileC2  ] ; then
        echo "C: $fileC2"
      fi
      done
fi
