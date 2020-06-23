### Find out what changes to port

* Start with the last working version. Say ``1.18``
* Check out HIXS applied tag/branch for that
```bash
    git co hixs-1.18.0
```
* Find out relevant files:
```bash
    for F in $(grep -Ri hixs * | \
    	cut -f1 -d":" | \
		sort -u | \
		grep configure |
		grep -v bad | \
		grep -vE 'gure$'\
	); do \
		echo -n "$F : "; \
		(git log --oneline $F | head -n1); \
	done
```
* To find out which directories to run auto-tools on, check out hixs-1.18.0 (as it
  has traces of a run checked in) and:
```bash
    for D in $(
    	grep -Ri hixs * | \
		cut -f1 -d":" | \
		sort -u | \
		grep autom4te.cache); do \
			echo $(dirname $(dirname $D));\
		done | sort -u
```
* Checkout the work-branch you were working with (sat 2.1.0)
* File by file & for each directory diff and apply relevant parts (search
  for "hixs"). For example like this:
```bash
    git difftool 14de55c -- configure.ac
```
* Check in your changes **before** running auto-tools
* Run auto-tools on the directories. Order does not matter but version
  might. This version was known to work ``autoconf2.64``. Run it explicitly
  if you have more than this version installed.

