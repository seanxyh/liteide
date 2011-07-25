package main

var listHTML = `
<!-- Golang Package List -->
<h2 id="title">Golang package list</h2>
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
		<td align="left" colspan="{Height|html-esc}"><a href="{Path|path-esc}">{Name|html-esc}</a></td>
		<td></td>
		<td align="left">{Synopsis|html-esc}</td>
		</tr>
	{.end}
	</table>
	</p>
{.end}
`

var listText = `
$list
{.section Dirs}
{.repeated section List}
{Path|path-esc}
{.end}
{.end}
`

var listLite = `
$list{.section Dirs}{.repeated section List},{Path|path-esc}{.end}{.end}
`

var findHTML = `
<!-- Golang Package List -->
{.section Find}
<h2 id="title">Find golang package "{@|html-esc}"</h2>
{.end}
{.section Dirs}
	{# DirList entries are numbers and strings - no need for FSet}
	<p>
	<table class="layout">
	<tr>
	<th align="left" colspan="{MaxHeight|html-esc}">Best</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	</tr>
	<tr>
{.end}	
{.section Best}
	<td align="left"><a href="{Best.Path|path-esc}">{@Best.Path|html-esc}</a></td>
	<td></td>
	<td align="left">{@Best.Synopsis|html-esc}</td>
	</tr>
	<tr>
{.end}	
{.section Dirs}
	<th align="left" colspan="{MaxHeight|html-esc}">Match</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	</tr>
	{.repeated section List}
		<tr>
		{Depth|padding}
		<td align="left" colspan="{Height|html-esc}"><a href="{Path|path-esc}">{Name|html-esc}</a></td>
		<td></td>
		<td align="left">{Synopsis|html-esc}</td>
		</tr>
	{.end}
	</table>
	</p>
{.end}
`
var findText = `
$best
{.section Best}
{Path|path-esc}
{.end}
$list
{.section Dirs}
{.repeated section List}
{Path|path-esc}
{.end}
{.end}
`

var findLite = `
$find,{.section Best}{Path|path-esc}{.end}{.section Dirs}{.repeated section List},{Path|path-esc}{.end}{.end}
`