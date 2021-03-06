import sys
import subprocess

top_srcdir = './'
if len(sys.argv) == 2:
    top_srcdir = sys.argv[1] + '/'


def write_version(branch, mmp, ghash, status):
    version_str = "#define GIT_VERSION \"{%s} %s %s\"\n" % \
                  (branch, ghash, status)

    if mmp:
        mmp_str = "#define PSI_VERSION \"%s\"\n" % (mmp)
    else:
        mmp_str = "#define PSI_VERSION \"%s\"\n" % ('(no tag)')

    with open('gitversion.h.tmp', 'w') as handle:
        handle.write(version_str)
        handle.write(mmp_str)


# Use Git to find current branch name
#   Returns "refs/heads/BRANCHNAME"
#   Use [11:] to skip refs/heads/
try:
    command = "git symbolic-ref -q HEAD"
    process = subprocess.Popen(command.split(), stderr=subprocess.PIPE,
                               stdout=subprocess.PIPE, cwd=top_srcdir,
                               universal_newlines=True)
    (out, err) = process.communicate()
    branch = str(out).rstrip()[11:]
    if process.returncode:
        branch = "detached?"
except:
    branch = "detached?"

# Use Git to find a sortable latest version number
try:
    command = "git describe --long --dirty --always"
    process = subprocess.Popen(command.split(), stderr=subprocess.PIPE,
                               stdout=subprocess.PIPE, cwd=top_srcdir,
                               universal_newlines=True)
    (out, err) = process.communicate()
    fields = str(out).rstrip().split('-')

    #         a68d223        # tags not pulled, clean git directory
    #         a68d223-dirty  # tags not pulled, changes to git-controlled files
    # 0.1-62-ga68d223        # tags pulled, clean git directory
    # 0.1-62-ga68d223-dirty  # tags pulled, changes to git-controlled files

    if fields[-1] == 'dirty':
        status = fields.pop()
    else:
        status = ''

    if len(fields[-1]) == 7:
        ghash = fields.pop()
    elif len(fields[-1]) == 8:
        ghash = fields.pop()[1:]
    else:
        ghash = ''

    # major-minor-patch
    if len(fields) == 2:
        mmp = '.'.join(fields)
    else:
        mmp = ''

    if process.returncode:
        try:
            # try to get some minimal version info from tarball not under git control
            zipname = top_srcdir.split('/')[-2]  # ending slash guaranteed
            command = "unzip -z ../" + zipname
            process = subprocess.Popen(command.split(), stderr=subprocess.PIPE,
                                       stdout=subprocess.PIPE, cwd=top_srcdir,
                                       universal_newlines=True)
            (out, err) = process.communicate()
            fields = str(out).rstrip().split()

            if zipname.endswith('master'):
                branch = 'master'
            status = ''
            ghash = fields.pop()[:7]
            mmp = ''

        except:
            status = ''
            ghash = ''
            mmp = ''

except:
    status = ''
    ghash = ''
    mmp = ''

write_version(branch, mmp, ghash, status)
