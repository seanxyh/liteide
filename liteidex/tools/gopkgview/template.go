package main

var packageHTML = `
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8"/>
  <title>The Go Programming Language</title>
<link rel="stylesheet" href="golangdoc/all.css" type="text/css" media="all" charset="utf-8">
</head>
<!--
	Copyright 2009 The Go Authors. All rights reserved.
	Use of this source code is governed by a BSD-style
	license that can be found in the LICENSE file.
-->
<body>
<div id="container">
<div id="topnav">
<h2 id="title">The Golang Document Package List</h2>
{.section Dirs}
	{# DirList entries are numbers and strings - no need for FSet}
	<p>
	<table class="layout">
	<tr>
	<th align="left" colspan="{MaxHeight|html-esc}">Name</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	</tr>
	{.repeated section List}
		<tr>
		{Depth|padding}
		<td align="left" colspan="{Height|html-esc}"><a href="{Path|html-esc}">{Name|html-esc}</a></td>
		<td></td>
		<td align="left">{Synopsis|html-esc}</td>
		</tr>
	{.end}
	</table>
	</p>
{.end}
</body>
</html>
`

var packageText = `
{.section Dirs}
{.repeated section List}
{Path|html-esc}
{.end}
{.end}
`