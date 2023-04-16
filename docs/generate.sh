#!/bin/bash

set -euo pipefail

INPUT="../README.md"
CONFIG="metadata.yaml"
OUTPUT="uxp1a.pdf"

help () {
    cat << EOF
Usage: generate.sh [OPTION]...
    -h --help        Display this message
    -p --packages    Install necessary packages first
    -i --input       Specify input markdown file
    -c --config      Specify input configuration file
    -o --output      Specify output pdf file
EOF
exit 0;
}

install () {
    sudo apt install texlive-full pandoc pandoc-citeproc npm -y
    sudo npm i -g mermaid-filter || echo "Error downloading mermaid-filter"
}

while [[ $# -gt 0 ]]; do
  case $1 in
    -h|--help)
        help
        ;;
    -p|--packages)
        install
        shift
        ;;
    -i|--input)
        INPUT="$2"
        shift
        shift
        ;;
    -c|--config)
        CONFIG="$2"
        shift
        shift
        ;;
    -o|--output)
        OUTPUT="$2"
        shift
        shift
        ;;
  esac
done

mkdir -p output/

pandoc $INPUT \
    --verbose \
    --standalone \
    --biblatex -o output/$OUTPUT \
    --listings \
    --highlight=tango \
    --metadata-file=$CONFIG \
    --pdf-engine=latexmk \
    --from markdown \
    -F mermaid-filter \
    --template include/wut-template.tex

rm mermaid-filter.err
