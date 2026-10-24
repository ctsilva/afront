"""Software renderer for README figures: shaded triangle meshes with optional
wireframe, and point clouds, drawn into a z-buffer with numpy.

usage: render_mesh.py out.png [--size W H] [--view AZ EL] [--up y|z] panel [panel ...]
  panel = file[:title][:opts]   opts: wire, nowire, points, color=#rrggbb
Reads .m (afront), .off, .obj (points). Panels share the camera (fit to the first).
"""
import sys, re, numpy as np
import matplotlib; matplotlib.use("Agg")
import matplotlib.pyplot as plt

def read_mesh(fn):
    V=[]; F=[]; vid={}
    if fn.endswith('.m'):
        for line in open(fn):
            p=line.split()
            if not p: continue
            if p[0]=='Vertex': vid[int(p[1])]=len(V); V.append([float(x) for x in p[2:5]])
            elif p[0]=='Face': F.append([int(x) for x in p[2:5]])
        F=[[vid[i] for i in f] for f in F]
    elif fn.endswith('.off'):
        f=open(fn); l=f.readline()
        while l.strip()=='' or l.startswith('#') or l.strip()=='OFF': l=f.readline()
        nv,nf=[int(x) for x in l.split()[:2]]
        for _ in range(nv): V.append([float(x) for x in f.readline().split()[:3]])
        for _ in range(nf):
            p=f.readline().split()
            if int(p[0])==3: F.append([int(x) for x in p[1:4]])
    elif fn.endswith('.obj'):
        for line in open(fn):
            p=line.split()
            if p and p[0]=='v': V.append([float(x) for x in p[1:4]])
            elif p and p[0]=='f': F.append([int(x.split('/')[0])-1 for x in p[1:4]])
    return np.array(V,float), np.array(F,int).reshape(-1,3)

def camera(az, el, up):
    az,el=np.radians(az),np.radians(el)
    if up=='y':
        d=np.array([np.cos(el)*np.sin(az), np.sin(el), np.cos(el)*np.cos(az)]); upv=np.array([0,1.,0])
    else:
        d=np.array([np.cos(el)*np.cos(az), np.cos(el)*np.sin(az), np.sin(el)]); upv=np.array([0,0,1.])
    z=d/np.linalg.norm(d); x=np.cross(upv,z); x/=np.linalg.norm(x); y=np.cross(z,x)
    return np.stack([x,y,z])   # rows: right, up, toward-viewer

def hexcol(s): s=s.lstrip('#'); return np.array([int(s[i:i+2],16)/255 for i in (0,2,4)])

def render(V,F,R,center,radius,W,H,color,wire,points,bg):
    img=np.ones((H,W,3))*bg; zb=np.full((H,W),-np.inf)
    P=(V-center)@R.T; scale=radius   # radius = pixels per unit, computed by main()
    X=P[:,0]*scale+W/2; Y=H/2-P[:,1]*scale; Z=P[:,2]
    zrange=Z.max()-Z.min()+1e-12
    light=np.array([0.3,0.6,0.74]); light/=np.linalg.norm(light)
    light2=np.array([-0.6,-0.2,0.3]); light2/=np.linalg.norm(light2)
    if points or len(F)==0:
        order=np.argsort(Z)
        for i in order:
            xi,yi=int(round(X[i])),int(round(Y[i]))
            if 1<=xi<W-1 and 1<=yi<H-1: img[yi-1:yi+2,xi-1:xi+2]=color*0.9
        return img
    # face normals in view space
    A,B,C=P[F[:,0]],P[F[:,1]],P[F[:,2]]
    N=np.cross(B-A,C-A); nl=np.linalg.norm(N,axis=1); N=N/np.maximum(nl,1e-30)[:,None]
    N[N[:,2]<0]*=-1   # two-sided
    lam=np.clip(N@light,0,1); lam2=np.clip(N@light2,0,1)
    shade=(0.36+0.56*lam+0.16*lam2)[:,None]*color[None,:]
    shade=np.clip(shade,0,1)
    xs=np.stack([X[F[:,0]],X[F[:,1]],X[F[:,2]]],1); ys=np.stack([Y[F[:,0]],Y[F[:,1]],Y[F[:,2]]],1); zs=np.stack([Z[F[:,0]],Z[F[:,1]],Z[F[:,2]]],1)
    for t in range(len(F)):
        x0,x1,x2=xs[t]; y0,y1,y2=ys[t]
        xmin,xmax=int(max(np.floor(min(x0,x1,x2)),0)),int(min(np.ceil(max(x0,x1,x2)),W-1))
        ymin,ymax=int(max(np.floor(min(y0,y1,y2)),0)),int(min(np.ceil(max(y0,y1,y2)),H-1))
        if xmin>xmax or ymin>ymax: continue
        det=(x1-x0)*(y2-y0)-(x2-x0)*(y1-y0)
        if abs(det)<1e-12: continue
        gx,gy=np.meshgrid(np.arange(xmin,xmax+1)+0.5,np.arange(ymin,ymax+1)+0.5)
        w1=((gx-x0)*(y2-y0)-(x2-x0)*(gy-y0))/det; w2=((x1-x0)*(gy-y0)-(gx-x0)*(y1-y0))/det; w0=1-w1-w2
        inside=(w0>=-1e-6)&(w1>=-1e-6)&(w2>=-1e-6)
        if not inside.any(): continue
        z=w0*zs[t,0]+w1*zs[t,1]+w2*zs[t,2]
        sub=zb[ymin:ymax+1,xmin:xmax+1]; upd=inside&(z>sub)
        sub[upd]=z[upd]; img[ymin:ymax+1,xmin:xmax+1][upd]=shade[t]
    if wire:
        E=np.concatenate([F[:,[0,1]],F[:,[1,2]],F[:,[2,0]]]); E=np.unique(np.sort(E,1),axis=0)
        L=np.hypot(X[E[:,0]]-X[E[:,1]],Y[E[:,0]]-Y[E[:,1]]); n=np.maximum(2,np.ceil(L*1.5).astype(int))
        ec=np.clip(color*0.38,0,1)
        for k in range(len(E)):
            if L[k]<2.5: continue
            t=np.linspace(0,1,n[k]); a,b=E[k]
            px=np.round(X[a]+(X[b]-X[a])*t).astype(int); py=np.round(Y[a]+(Y[b]-Y[a])*t).astype(int); pz=Z[a]+(Z[b]-Z[a])*t
            ok=(px>=0)&(px<W)&(py>=0)&(py<H); px,py,pz=px[ok],py[ok],pz[ok]
            vis=pz>=zb[py,px]-zrange*0.004
            img[py[vis],px[vis]]=0.45*img[py[vis],px[vis]]+0.55*ec
    return img

def main():
    args=sys.argv[1:]; out=args.pop(0); W,H=900,900; az,el=35,20; up='y'
    while args and args[0].startswith('--'):
        k=args.pop(0)
        if k=='--size': W,H=int(args.pop(0)),int(args.pop(0))
        elif k=='--view': az,el=float(args.pop(0)),float(args.pop(0))
        elif k=='--up': up=args.pop(0)
    panels=[]
    for a in args:
        parts=a.split(':'); fn=parts[0]; title=parts[1] if len(parts)>1 else ''; opts=parts[2:]
        panels.append((fn,title,opts))
    R=camera(az,el,up); bg=np.array([0.988,0.988,0.984])
    V0,_=read_mesh(panels[0][0]); P0=V0@R.T; lo,hi=P0.min(0),P0.max(0)
    center=R.T@((lo+hi)/2); radius=0.90*min(W/(hi[0]-lo[0]), H/(hi[1]-lo[1]))
    fig,axes=plt.subplots(1,len(panels),figsize=(W/100*len(panels),H/100+0.45),facecolor=bg)
    if len(panels)==1: axes=[axes]
    for ax,(fn,title,opts) in zip(axes,panels):
        V,F=read_mesh(fn); color=hexcol('#b7c5da'); wire='wire' in opts; pts='points' in opts
        for o in opts:
            if o.startswith('color='): color=hexcol(o[6:])
        img=render(V,F,R,center,radius,W,H,color,wire,pts,bg)
        ax.imshow(img,interpolation='lanczos'); ax.set_axis_off()
        nv,nf=len(V),len(F)
        ax.set_title(title.replace('{V}',f'{nv:,}').replace('{F}',f'{nf:,}'),fontsize=11,color='#2b2b2b',pad=8)
        print(fn, nv, nf)
    plt.subplots_adjust(left=0.01,right=0.99,top=0.93,bottom=0.01,wspace=0.02)
    fig.savefig(out,dpi=100,facecolor=bg); print('wrote',out)
main()
