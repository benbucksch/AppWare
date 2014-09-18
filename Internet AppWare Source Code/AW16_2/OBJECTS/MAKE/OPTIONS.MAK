# Compile flags that are common to all alm's for their release versions
################################################################
# -2   = 286 code
# -c   = compile only
# -ml! = large, DS != SS
# -Vf  = far virtual tables
# -V   = one instance of virtual table or non-expanded inline function
# -w   = all warnings on
# -WDE = Windows DLL, explicit functions exportable
# -k-   = no standard stack frame
# -O... = optimizations
# -v-   = no debug
# -vi   = expand inline
# -X    = suppress auto-dependency output
# -Z    = redundant register load optimization
# Cannot use -Om to move invariant code out of loops without causing
#  problems.
-2 -c -ml! -Vf -V -w -WDE -k- -O -Obegilpvs -v- -vi -X -Z

################################################################
#
# Linker flags
#

# -c   = case-sensitive link
# -C   = EXPORTS & IMPORTS are case-sensitive
# -x   = no map file
# -P-  = don't pack code segments
# -Twd = Windows' DLL


#resource compiler flags
RCFLAGS_G = -31 -t


#local options for ALM_NWDS.DLL
-g150 -DNWWIN

#we're pulling some of the netware .lib's off of Q
# Q: is mapped to MAP ROOT Q:=MUR-SERIUS1\SYS:DEVTOOLS\NWCLNT41.SDK\BLD\940225\CLNT

