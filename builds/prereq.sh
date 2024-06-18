#!/bin/bash

if [ "$ENVSET" == "1" ]; then
  exit 0
fi

if [ "$1" == "build" ]; then

  case $2 in
    lux)
      if ! module is-loaded gcc; then
        echo "modulefile required: gcc"
        echo "do: 'module load gcc'"
        exit 1
      fi
      ;;
    dag)
       if ! brew list gcc ; then
         echo "do: 'brew install gcc hdf5'"
         exit 1
       fi
     ;;
  esac

fi

exit 0