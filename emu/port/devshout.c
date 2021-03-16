#include	"dat.h"
#include	"fns.h"
#include	"error.h"

enum {
	Qdir,
	Qdata,
	Qctl,
};

Dirtab shouttab[] =
{
	".",         {Qdir,	0, QTDIR}, 0,	0555,
	"shoutdata", {Qdata},          0,	0666,
	"shoutctl",	 {Qctl},           0,	0666,
};

enum {
	CMerase
};

static
Cmdtab shoutcmd[] = {
	CMerase, "erase", 0,	/* known below to be first, to cope with stepN */
};

static u_int8_t *shoutdata;

static Chan *
shoutattach(char *spec)
{
	return devattach('S', spec);
}

static Walkqid *
shoutwalk(Chan *c, Chan *nc, char **name, int nname)
{
	return devwalk(c, nc, name, nname, shouttab, nelem(shouttab), devgen);
}

static int
shoutstat(Chan *c, uchar *db, int n)
{
	return devstat(c, db, n, shouttab, nelem(shouttab), devgen);
}

static Chan *
shoutopen(Chan *c, int omode)
{
	return devopen(c, omode, shouttab, nelem(shouttab), devgen);
}

static void
shoutclose(Chan *c)
{
	USED(c)
	/*
	if((c->flag & COPEN) == 0)
		return;
	if(c->qid.path == Qdata)
		free(shoutdata);
	*/
}

static long
shoutread(Chan *c, void *va, long count, vlong offset)
{
	switch((ulong)c->qid.path){
	case Qdir:
		return devdirread(c, va, count, shouttab, nelem(shouttab), devgen);
	case Qdata:
		if(shoutdata != nil)
			count = readstr(offset, va, count, (char *)shoutdata);
		else
			count = 0;
		break;
	default:
		count = 0;
		break;
	}
	return count;
}

static long
shoutwrite(Chan *c, void *va, long count, vlong offset)
{
	Cmdbuf *cb;
	Cmdtab *ct;
	char *p;
	u_int8_t *data;
	switch((ulong)c->qid.path){
	case Qctl:
		cb = parsecmd(va, count);
		ct = lookupcmd(cb, shoutcmd, nelem(shoutcmd));
		if(shoutdata != nil && ct->index == CMerase){
			shouttab[1].length = 0;
			free(shoutdata);
			shoutdata = nil;
		}
		break;
	case Qdata:
		data = (u_int8_t *)va;
		if(shoutdata == nil){
			shouttab[1].length = offset + count;
			shoutdata = malloc(shouttab[1].length+1);
		}

		for(long i = 0; i < count; i++){
			if(data[i] >= 97 && data[i] <= 122)
				data[i] = data[i] - 32;
			else
				data[i] = data[i];
		}

		if(count + offset > shouttab[1].length){
			shouttab[1].length = count + offset;
			shoutdata = realloc(c->aux, shouttab[1].length+1);
		}

		if((p = shoutdata) == nil)
			error(Enovmem);
		memmove(p+offset, (void *)data, count);
		shouttab[1].qid.vers++;
		break;
	default:
		error(Ebadusefd);
	}

	return count;
}

Dev shoutdevtab = {
	'S',
	"shout",

	devinit,
	shoutattach,
	shoutwalk,
	shoutstat,
	shoutopen,
	devcreate,
	shoutclose,
	shoutread,
	devbread,
	shoutwrite,
	devbwrite,
	devremove,
	devwstat,
};
