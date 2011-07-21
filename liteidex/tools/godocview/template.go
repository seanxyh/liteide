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
		<td align="left" colspan="{Height|html-esc}"><a href="{Path|html-esc}">{Name|html-esc}</a></td>
		<td></td>
		<td align="left">{Synopsis|html-esc}</td>
		</tr>
	{.end}
	</table>
	</p>
{.end}
`

var listText = `
{.section Dirs}
{.repeated section List}
{Path|path-esc}
{.end}
{.end}
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
	{.section Best}
		<tr>
		<td width="0"></td>
		<td align="left" colspan="{Height|html-esc}"><a href="{Path|html-esc}">{Path|path-esc}</a></td>
		<td></td>
		<td align="left">{Synopsis|html-esc}</td>
		</tr>
	{.end}	
	<tr>
	<th align="left" colspan="{MaxHeight|html-esc}">Matches</th>
	<td width="25">&nbsp;</td>
	<th align="left">Synopsis</th>
	</tr>
	{.repeated section List}
		<tr>
		<td width="0"></td>
		<td align="left" colspan="{Height|html-esc}"><a href="{Path|html-esc}">{Path|path-esc}</a></td>
		<td></td>
		<td align="left">{Synopsis|html-esc}</td>
		</tr>
	{.end}
	</table>
	</p>
{.end}
`

var findText = `
{.section Best}{Path|path-esc}{.end},{.section Dirs}{.repeated section List}{Path|path-esc},{.end}{.end}
`