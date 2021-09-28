# Lookup Table

```asm
0000000000000e24 <lookup_decode>:
     e24:	55                   	push   rbp
     e25:	48 89 e5             	mov    rbp,rsp
     e28:	89 f8                	mov    eax,edi
     e2a:	88 45 fc             	mov    BYTE PTR [rbp-0x4],al
     e2d:	0f b6 45 fc          	movzx  eax,BYTE PTR [rbp-0x4]
     e31:	48 63 d0             	movsxd rdx,eax
     e34:	48 8d 05 25 12 20 00 	lea    rax,[rip+0x201225]        # 202060 <decode_lookup_tbl>
     e3b:	0f b6 04 02          	movzx  eax,BYTE PTR [rdx+rax*1]
     e3f:	5d                   	pop    rbp
     e40:	c3                   	ret
```

# Polynomial

```asm
0000000000000e41 <poly_decode>:
     e41:	55                   	push   rbp
     e42:	48 89 e5             	mov    rbp,rsp
     e45:	89 f8                	mov    eax,edi
     e47:	88 45 fc             	mov    BYTE PTR [rbp-0x4],al
     e4a:	0f b6 45 fc          	movzx  eax,BYTE PTR [rbp-0x4]
     e4e:	f2 0f 2a c0          	cvtsi2sd xmm0,eax
     e52:	f2 0f 10 0d 86 03 00 	movsd  xmm1,QWORD PTR [rip+0x386]        # 11e0 <_IO_stdin_used+0xf0>
     e59:	00 
     e5a:	f2 0f 59 c1          	mulsd  xmm0,xmm1
     e5e:	f2 0f 10 0d 82 03 00 	movsd  xmm1,QWORD PTR [rip+0x382]        # 11e8 <_IO_stdin_used+0xf8>
     e65:	00 
     e66:	f2 0f 5c c1          	subsd  xmm0,xmm1
     e6a:	66 0f 28 c8          	movapd xmm1,xmm0
     e6e:	0f b6 55 fc          	movzx  edx,BYTE PTR [rbp-0x4]
     e72:	0f b6 45 fc          	movzx  eax,BYTE PTR [rbp-0x4]
     e76:	0f af c2             	imul   eax,edx
     e79:	f2 0f 2a c0          	cvtsi2sd xmm0,eax
     e7d:	f2 0f 10 15 6b 03 00 	movsd  xmm2,QWORD PTR [rip+0x36b]        # 11f0 <_IO_stdin_used+0x100>
     e84:	00 
     e85:	f2 0f 59 c2          	mulsd  xmm0,xmm2
     e89:	f2 0f 58 c8          	addsd  xmm1,xmm0
     e8d:	0f b6 55 fc          	movzx  edx,BYTE PTR [rbp-0x4]
     e91:	0f b6 45 fc          	movzx  eax,BYTE PTR [rbp-0x4]
     e95:	0f af d0             	imul   edx,eax
     e98:	0f b6 45 fc          	movzx  eax,BYTE PTR [rbp-0x4]
     e9c:	0f af c2             	imul   eax,edx
     e9f:	f2 0f 2a c0          	cvtsi2sd xmm0,eax
     ea3:	f2 0f 10 15 4d 03 00 	movsd  xmm2,QWORD PTR [rip+0x34d]        # 11f8 <_IO_stdin_used+0x108>
     eaa:	00 
     eab:	f2 0f 59 c2          	mulsd  xmm0,xmm2
     eaf:	f2 0f 58 c1          	addsd  xmm0,xmm1
     eb3:	f2 0f 2c c0          	cvttsd2si eax,xmm0
     eb7:	5d                   	pop    rbp
     eb8:	c3                   	ret    
```

# Switch

```asm
0000000000000eb9 <switch_decode>:
     eb9:	55                   	push   rbp
     eba:	48 89 e5             	mov    rbp,rsp
     ebd:	89 f8                	mov    eax,edi
     ebf:	88 45 fc             	mov    BYTE PTR [rbp-0x4],al
     ec2:	0f b6 45 fc          	movzx  eax,BYTE PTR [rbp-0x4]
     ec6:	83 f8 0a             	cmp    eax,0xa
     ec9:	74 1f                	je     eea <switch_decode+0x31>
     ecb:	83 f8 0a             	cmp    eax,0xa
     ece:	7f 07                	jg     ed7 <switch_decode+0x1e>
     ed0:	83 f8 09             	cmp    eax,0x9
     ed3:	74 0e                	je     ee3 <switch_decode+0x2a>
     ed5:	eb 28                	jmp    eff <switch_decode+0x46>
     ed7:	83 f8 0d             	cmp    eax,0xd
     eda:	74 15                	je     ef1 <switch_decode+0x38>
     edc:	83 f8 20             	cmp    eax,0x20
     edf:	74 17                	je     ef8 <switch_decode+0x3f>
     ee1:	eb 1c                	jmp    eff <switch_decode+0x46>
     ee3:	b8 00 00 00 00       	mov    eax,0x0
     ee8:	eb 1a                	jmp    f04 <switch_decode+0x4b>
     eea:	b8 01 00 00 00       	mov    eax,0x1
     eef:	eb 13                	jmp    f04 <switch_decode+0x4b>
     ef1:	b8 02 00 00 00       	mov    eax,0x2
     ef6:	eb 0c                	jmp    f04 <switch_decode+0x4b>
     ef8:	b8 03 00 00 00       	mov    eax,0x3
     efd:	eb 05                	jmp    f04 <switch_decode+0x4b>
     eff:	b8 ff ff ff ff       	mov    eax,0xffffffff
     f04:	5d                   	pop    rbp
     f05:	c3                   	ret
```
