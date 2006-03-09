<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

  <xsl:output method="html" indent="yes"/>
  
  <xsl:template match="html">
    <qt>
      <xsl:attribute name="title"><xsl:value-of select="normalize-space(head/title)"/></xsl:attribute>
      <xsl:apply-templates select="node()"/>
    </qt>
  </xsl:template>
 
  <xsl:template match="head">
  </xsl:template>
 
  <xsl:template match="body">
    <xsl:apply-templates select="node()"/>
  </xsl:template>

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

</xsl:stylesheet>
