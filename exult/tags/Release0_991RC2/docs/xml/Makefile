CLASSPATH = xml-apis.jar:xercesImpl.jar:xalan.jar
JAVA = java
INPUT = readme.xml faq.xml
CLASS = org.apache.xalan.xslt.Process

HTML_OBJS := $(patsubst %.xml,%.html,$(wildcard *.xml))
TEXT_OBJS := $(patsubst %.xml,%.txt,$(wildcard *.xml))
PHP_OBJS := $(patsubst %.xml,%.dat,$(wildcard *.xml))

all: html text php ReadMe.html

# Rules for transforming the XML source
.xml.html: html.xsl
	$(JAVA) -classpath $(CLASSPATH) $(CLASS) -IN $< -XSL html.xsl -OUT $@
.xml.txt: text.xsl
	$(JAVA) -classpath $(CLASSPATH) $(CLASS) -IN $< -XSL text.xsl -OUT $@
.xml.dat: php.xsl
	$(JAVA) -classpath $(CLASSPATH) $(CLASS) -IN $< -XSL php.xsl -OUT $@
.SUFFIXES: .xml .html .txt .dat

# rule for renaming docs.html to ReadMe.html
ReadMe.html: docs.html
	mv docs.html ReadMe.html

# remove all generated files
clean:
	-rm -f *.html *.txt *.dat

html: $(HTML_OBJS)
text: $(TEXT_OBJS)
php: $(PHP_OBJS)

.PHONY: all html text php clean
