CLASSPATH = xerces.jar:xalan.jar
JAVA = java
INPUT = readme.xml
CLASS = org.apache.xalan.xslt.Process

all: html text php


html: $(INPUT) readme_html.xsl
	$(JAVA) -classpath $(CLASSPATH) $(CLASS) -IN $(INPUT) -XSL readme_html.xsl -OUT docs.html

text: $(INPUT) readme_text.xsl
	$(JAVA) -classpath $(CLASSPATH) $(CLASS) -IN $(INPUT) -XSL readme_text.xsl -OUT readme.txt

php: $(INPUT) readme_php.xsl
	$(JAVA) -classpath $(CLASSPATH) $(CLASS) -IN $(INPUT) -XSL readme_php.xsl -OUT docs.dat
