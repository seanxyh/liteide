package main

var packageHTML = `
<!-- Golang Package List -->
<h2 id="title">Golang Package List</h2>
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

var packageText = `
{.section Dirs}
{.repeated section List}
{Path|html-esc}
{.end}
{.end}
`