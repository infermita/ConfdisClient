#!/bin/sh
dh_make -s -c gpl -e info@gecoitalia.biz --createorig -p condis_1
cp service/condis.service debian/
sed -i "s/dh \$\@/dh \$\@ --with systemd/g" debian/rules
dpkg-buildpackage -us -uc
