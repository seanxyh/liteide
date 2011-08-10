package main

var listHTML = `
<!-- Golang Package List -->
<p class="detail">
Need more packages? The
<a href="http://godashboard.appspot.com/package">Package Dashboard</a>
provides a list of <a href="/cmd/goinstall/">goinstallable</a> packages.
</p>
<h2 id="Subdirectories">Subdirectories</h2>
<p>
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
<p class="detail">
Need more packages? The
<a href="http://godashboard.appspot.com/package">Package Dashboard</a>
provides a list of <a href="/cmd/goinstall/">goinstallable</a> packages.
</p>
<h2 id="Subdirectories">Subdirectories</h2>
<p>{.section Dirs}
<p>
<table class="layout">
	<tr>
	<th align="left">Best</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	{.section Best}
		<tr>
		<td align="left"><a href="{Path|path-esc}">{Path|path-esc}</a></td>
		<td></td>
		<td align="left">{Synopsis|html-esc}</td>
		</tr>
	{.end}	
	<tr>
	<th align="left">Match</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	</tr>
	{.repeated section List}
		<tr>
		<td align="left"><a href="{Path|path-esc}">{Path|path-esc}</a></td>
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