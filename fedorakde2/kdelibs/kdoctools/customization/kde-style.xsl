<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
		xmlns:doc="http://nwalsh.com/xsl/documentation/1.0"
		version="1.0">

<xsl:param name="callout.graphics.path" select="'common/'" doc:type='string'/>

<xsl:template match="guilabel|guimenu|guisubmenu|guimenuitem|interface|guibutton">
  <span style="background-color: rgb(240,240,240); 
	color: rgb(0,0,0);">  
     <xsl:call-template name="inline.charseq"/>
  </span>
</xsl:template>

<xsl:template match="accel">
  <span style="background-color: rgb(240,240,240); 
	color: rgb(0,0,0); 
	text-decoration: underline;">  
     <xsl:call-template name="inline.charseq"/>
   </span>
</xsl:template>

<xsl:attribute-set name="kde.body.attrs">
</xsl:attribute-set>

<xsl:template match="command">
  <span class="command">
    <xsl:call-template name="inline.boldseq"/>
  </span>
</xsl:template>

<xsl:template match="option">
  <span class="option">
    <xsl:call-template name="inline.monoseq"/>
  </span>
</xsl:template>

<xsl:template match="parameter">
  <span class="parameter">
    <xsl:call-template name="inline.italicmonoseq"/>
  </span>
</xsl:template>

<xsl:template match="envar">
  <span class="envar">
    <xsl:call-template name="inline.monoseq"/>
  </span>
</xsl:template>

<xsl:template match="replaceable" priority="1">
  <span class="replaceable">
    <xsl:call-template name="inline.italicmonoseq"/>
  </span>
</xsl:template>


</xsl:stylesheet>
