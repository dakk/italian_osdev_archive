#!/bin/sh
#
#       Generate the Doxygen configuration file
#
#       Author: Andrea Righi <righiandr@users.sf.net>
#

# This script can be used only by the top makefile #
if [ "$MAKE" == "" ]; then
	echo "Only the top makefile can run this script!!!"
	exit 1
fi

echo -n "Generating $DOXYFILE... "
cat << EOF > $DOXYFILE
PROJECT_NAME           = Minirighi
PROJECT_NUMBER         = $VERSION
OUTPUT_DIRECTORY       = $DOC/alldocs
OUTPUT_LANGUAGE        = English
USE_WINDOWS_ENCODING   = NO
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ALWAYS_DETAILED_SEC    = YES
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = NO
STRIP_FROM_PATH        =
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = NO
MULTILINE_CPP_IS_BRIEF = YES
DETAILS_AT_TOP         = NO
INHERIT_DOCS           = NO
DISTRIBUTE_GROUP_DOC   = NO
TAB_SIZE               = 8
ALIASES                =
OPTIMIZE_OUTPUT_FOR_C  = YES
OPTIMIZE_OUTPUT_JAVA   = NO
SUBGROUPING            = YES
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = YES
EXTRACT_STATIC         = YES
EXTRACT_LOCAL_CLASSES  = NO
HIDE_UNDOC_MEMBERS     = NO
HIDE_UNDOC_CLASSES     = NO
HIDE_FRIEND_COMPOUNDS  = NO
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = NO
CASE_SENSE_NAMES       = YES
HIDE_SCOPE_NAMES       = NO
SHOW_INCLUDE_FILES     = YES
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = YES
GENERATE_BUGLIST       = YES
GENERATE_DEPRECATEDLIST= YES
ENABLED_SECTIONS       =
MAX_INITIALIZER_LINES  = 30
SHOW_USED_FILES        = YES
QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = YES
WARN_IF_DOC_ERROR      = YES
WARN_FORMAT            = "\$file:\$line: \$text"
WARN_LOGFILE           =
INPUT                  = $INCLUDE \\
                         $KERNEL
FILE_PATTERNS          = *.c \\
                         *.h
RECURSIVE              = YES
EXCLUDE                =
EXCLUDE_SYMLINKS       = NO
EXCLUDE_PATTERNS       =
EXAMPLE_PATH           = . \\
			 ../
EXAMPLE_PATTERNS       =
EXAMPLE_RECURSIVE      = NO
IMAGE_PATH             = $DOC/images
INPUT_FILTER           =
FILTER_SOURCE_FILES    = NO
SOURCE_BROWSER         = YES
INLINE_SOURCES         = YES
STRIP_CODE_COMMENTS    = NO
REFERENCED_BY_RELATION = NO
REFERENCES_RELATION    = NO
VERBATIM_HEADERS       = NO
ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          =
GENERATE_HTML          = YES
HTML_OUTPUT            = html
HTML_FILE_EXTENSION    = .html
HTML_ALIGN_MEMBERS     = YES
GENERATE_HTMLHELP      = NO
CHM_FILE               =
HHC_LOCATION           =
GENERATE_CHI           = NO
BINARY_TOC             = NO
TOC_EXPAND             = NO
DISABLE_INDEX          = NO
ENUM_VALUES_PER_LINE   = 4
GENERATE_TREEVIEW      = NO
TREEVIEW_WIDTH         = 250
GENERATE_LATEX         = NO 
LATEX_OUTPUT           = latex
LATEX_CMD_NAME         = latex
MAKEINDEX_CMD_NAME     = makeindex
COMPACT_LATEX          = NO
PAPER_TYPE             = a4wide
EXTRA_PACKAGES         =
LATEX_HEADER           =
PDF_HYPERLINKS         = YES
USE_PDFLATEX           = YES
LATEX_BATCHMODE        = YES 
LATEX_HIDE_INDICES     = NO
GENERATE_RTF           = NO
RTF_OUTPUT             = rtf
COMPACT_RTF            = NO
RTF_HYPERLINKS         = NO
RTF_STYLESHEET_FILE    =
RTF_EXTENSIONS_FILE    =
GENERATE_MAN           = NO
MAN_OUTPUT             = man
MAN_EXTENSION          = .3
MAN_LINKS              = YES
GENERATE_XML           = NO
XML_OUTPUT             = xml
XML_SCHEMA             =
XML_DTD                =
GENERATE_AUTOGEN_DEF   = NO
GENERATE_PERLMOD       = NO
PERLMOD_LATEX          = NO
PERLMOD_PRETTY         = YES
PERLMOD_MAKEVAR_PREFIX =
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = NO
EXPAND_ONLY_PREDEF     = NO
SEARCH_INCLUDES        = YES
INCLUDE_PATH           =
INCLUDE_FILE_PATTERNS  =
PREDEFINED             =
EXPAND_AS_DEFINED      =
SKIP_FUNCTION_MACROS   = NO
TAGFILES               =
GENERATE_TAGFILE       =
ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = YES
PERL_PATH              = /usr/bin/perl
CLASS_DIAGRAMS         = NO
HIDE_UNDOC_RELATIONS   = NO
HAVE_DOT               = NO
CLASS_GRAPH            = NO
COLLABORATION_GRAPH    = NO
UML_LOOK               = NO
TEMPLATE_RELATIONS     = NO
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CALL_GRAPH             = NO
GRAPHICAL_HIERARCHY    = YES
DOT_IMAGE_FORMAT       = png
DOT_PATH               =
DOTFILE_DIRS           =
MAX_DOT_GRAPH_WIDTH    = 1024
MAX_DOT_GRAPH_HEIGHT   = 1024
MAX_DOT_GRAPH_DEPTH    = 0
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
SEARCHENGINE           = NO

EOF

echo "done."
echo
