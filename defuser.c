#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t lo=(uint32_t)challenge;
    uint32_t hi=(uint32_t)(challenge>>32);
    uint32_t mid=(uint32_t)(challenge>>16);
    uint32_t s0=(lo+2u*hi)^0x3053u;
    uint32_t s1=(5u*hi+mid)^0xa2c3u;
    uint32_t s2=(3u*s1)^(lo+0x1dceu);
    values[0]=s2^s0;
    count=1;

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t v[6];
    int shift=28;
    v[0]=3*((challenge>>40)&0x1f);
    values[0]=v[0];
    for (int i=1;i<6;i++){
        v[i]=(((challenge>>shift)&0xf)+i)^(3*v[i-1]);
        values[i]=v[i];
        shift+=5;
    }
    count=6;
    return write_answer(out, cap, values, count);
}
static uint32_t p3_dispatch(uint32_t index, uint32_t x) {
    switch (index) {
        case 0: return (2u*x)^0x8874u;          /* p3_case_5 */
        case 1: return (4u*x)^0xbe63u;          /* p3_case_1 */
        case 2: return 5u*x+0x15531u;           /* p3_case_7 */
        case 3: return (x^0x56d4u)+(x>>2);    /* p3_case_2 */
        case 4: return (x^0x6780u)+(x>>1);    /* p3_case_6 */
        case 5: return 3u*x+0x899fu;            /* p3_case_4 */
        case 6: return 5u*x+0xe682u;            /* p3_case_3 */
        case 7: return 5u*x+0x2964u;            /* p3_case_0 */
        default: return 0;
    }
}
int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t v[2];
    v[0]=challenge&7u;
    uint32_t u=challenge>>3;
    int esi=(uint32_t)(challenge>>32)^u;
    values[0]=v[0];
    values[1]=p3_dispatch(v[0],esi);
    count=2;
    return write_answer(out, cap, values, count);
}
int p4_walk(int target,int low,int high,int salt){
    int mid=low+(high-low)/2;
    if(mid==target){
        return (mid+0xa7)^salt;
    }
    if(mid>target){
        uint32_t child=p4_walk(target,low,mid-1,salt);
        return ((5u*child)^(mid+salt))^0x12u;
    }
    uint32_t child=p4_walk(target,mid+1,high,salt);
    return ((5*child)^(mid+salt))^0x9u;
}
int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t target=challenge&0x7f;
    uint32_t low=0;
    uint32_t high=0x7f;
    uint32_t salt=((uint32_t)(challenge>>7))^((uint32_t)(challenge>>32));
    values[0]=p4_walk(target,low,high,salt);
    count=1;
    return write_answer(out, cap, values, count);
}
typedef struct P5Node{
    const struct P5Node *next;
    uint32_t tag;
    uint32_t value;
} P5Node;
static const P5Node N2={NULL,2421,65424};
static const P5Node N9={&N2,126,61776};
static const P5Node N11={&N9,1365,55355};
static const P5Node N1={&N11,2844,45547};
static const P5Node N0={&N1,797,7993};
static const P5Node N10={&N0,595,9322};
static const P5Node N7={&N10,1142,25759};
static const P5Node N3={&N7,327,58204};
static const P5Node N4={&N3,1907,24539};
static const P5Node N8={&N4,1574,28673};
static const P5Node N5={&N8,2678,1325};
static const P5Node N6={&N5,2130,8209};

static const P5Node *const p5_heads[8] = {
    &N6, &N5, &N8, &N4, &N3, &N7, &N10, &N0
};
static uint32_t p5_traverse(uint64_t challenge,uint32_t *out_tag){
    uint32_t idx=(uint32_t)challenge&7u;
    uint32_t steps=((uint32_t)(challenge>>3))&3u;
    const P5Node *node=p5_heads[idx];
    uint32_t acc=(uint32_t)(challenge>>6)
                ^(uint32_t)(challenge>>32)
                ^node->value
                ^0x9d8eu;
    acc+=node->tag;
    *out_tag=node->tag;
    for(uint32_t i=0;i<steps;++i){
        node=node->next;
        acc=(acc^node->value)+node->tag;
        *out_tag=node->tag;
    }
    return acc;
}
int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t tag;
    uint32_t acc=p5_traverse(challenge,&tag);
    values[0]=acc;
    values[1]=tag;
    count=2;
    return write_answer(out, cap, values, count);
}
static uint32_t ror32(uint32_t x,unsigned n){
    return(x>>n)|(x<<(32u-n));
}
static uint32_t rol32(uint32_t x,unsigned n){
    return(x<<n)|(x>>(32u-n));
}
static uint32_t p6_op_0(uint32_t x){return((x>>2)^x)+0x39b3u;}
static uint32_t p6_op_1(uint32_t x){return(x+0x9e5u)^(x>>2);}
static uint32_t p6_op_2(uint32_t x){return(x^0x440au)+(x<<5);}
static uint32_t p6_op_3(uint32_t x){return ror32(x,1);}
static uint32_t p6_op_4(uint32_t x){return rol32(x,3)^0xaff8u;}
static uint32_t p6_op_5(uint32_t x){return(0xb950u+8u*x)^(5u*x);}
typedef uint32_t(*P6Op)(uint32_t);
static P6Op const p6_ops[8]={
    p6_op_3, p6_op_4, p6_op_5, p6_op_5,
    p6_op_0, p6_op_2, p6_op_1, p6_op_2
};
static int p6_dispatch(uint64_t challenge){
    uint32_t mixed=(uint32_t)(challenge>>6)^(uint32_t)(challenge>>32);
    uint32_t idx1=(uint32_t)(challenge&7u);
    uint32_t idx2=((uint32_t)(challenge>>3))&7u;
    uint32_t x1=p6_ops[idx1](mixed);
    x1^=0x9866u;
    uint32_t x2=p6_ops[idx2](x1);
    return x2+0xa01cu;
}
int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    values[0]=p6_dispatch(challenge);
    count=1;
    return write_answer(out, cap, values, count);
}
int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    uint32_t r1=(uint32_t)context&3;
    uint32_t a=(uint32_t)challenge^(uint32_t)(challenge>>32);
    uint32_t b=(uint32_t)context^(uint32_t)(context>>32);
    uint32_t c=(uint32_t)(context>>16);
    uint32_t r9=(0xbda1*r1)^c;
    uint32_t m=((rol32(a^b,3)+0xbda1)^0xd4e8);
    uint32_t v0,v1,v2;
    v0=r1;
    v1=m^r9;
    v2=(m^r9)^(0xd4e8+b+r1+(a>>3));
    int n=snprintf(out,cap,"%32sOPEN %u %u %u","",v0,v1,v2);
    if(n<0||(size_t)n>=cap)return 1;
    return 0;
}
