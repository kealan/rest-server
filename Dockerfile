# Dockerfile
#
#
# @author      Kealan McCusker <kealanmccusker@gmail.com>
# -----------------------------------------------------------------------------

FROM scratch
ADD main /
CMD ["/main"]
