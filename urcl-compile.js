const { throws } = require('assert');
const fs = require('fs');

const is = {
    addrr: 0,
    addir: 1,
    addri: 2,
    subrr: 3,
    subir: 4,
    subii: 5,
    subri: 100,
    rshr: 6,
    rshl: 7,
    lshr: 8,
    lshl: 9,
    incr: 10,
    incl: 11,
    decr: 12,
    decl: 13,
    xorrr: 14,
    xorlr: 15,
    xorll: 16,
    orrr: 17,
    orlr: 18,
    orll: 19,
    andrr: 20,
    andlr: 21,
    andll: 22,
    notr: 23,
    notl: 24,
    mov: 25,
    imm: 26,
    lodr: 27,
    lodl: 28,
    stral: 29,
    strar: 30,
    strrl: 31,
    strrr: 32,
    brar: 33,
    bral: 34,
    brcr: 35,
    brcl: 36,
    bncr: 37,
    bncl: 38,
    brzr: 39,
    brzl: 40,
    bnzr: 41,
    bnzl: 42,
    brnl: 56,
    brnr: 57,
    brpl: 58,
    brpr: 59,
    nop: 43,
    hlt: 44,
    pshl: 45,
    pshr: 46,
    popi: 47,
    calr: 48,
    call: 49,
    ret: 50,
    sav: 51,
    rsr: 52,
    in: 53,
    outr: 54,
    outl: 55,
    mltrr: 60,
    mltlr: 61,
    mltll: 62,
    divrr: 63,
    divlr: 64,
    divrl: 65,
    divll: 66,
    modrr: 67,
    modlr: 68,
    modrl: 69,
    modll: 70,
    brllrr: 71,
    brllrl: 72,
    brlllr: 73,
    brlrrr: 74,
    brlrrl: 75,
    brlrlr: 76,
    brglrr: 77,
    brglrl: 78,
    brgllr: 79,
    brgrrr: 80,
    brgrrl: 81,
    brgrlr: 82,
    brelrr: 83,
    brelrl: 84,
    brerrr: 85,
    brerrl: 86,
    bnelrr: 87,
    bnelrl: 88,
    bnerrr: 89,
    bnerrl: 90,
    bodl: 91,
    bodr: 92,
    bevl: 93,
    bevr: 94,
}

/**
 * @param {string} s
 * @returns {string[]}
 */

const split = (s) => {
    let n = '';
    let t = [];
    for(let c of s) {
        if(c == ' ' || c == '\t' || c == '\r') {
            if(n) t.push(n);
            n = '';
        }
        else if(c == ',') {
            if(n) t.push(n);
            t.push(',');
            n = '';
        }
        else if(c == '\n') {
            if(n) t.push(n);
            t.push('\n');
            n = '';
        }
        else if(c == '"') {
            if(n) t.push(n);
            t.push('"');
            n = ''
        }
        else if(c == ';') {
            if(n) t.push(n);
            t.push(';');
            n = '';
        }
        else if(c == '/') {
            if(n == '/') {
                t.push('//');
                n = '';
            } else if(n == '') n = '/';
            else {
                if(n) t.push(n);
                n = '/'
            }
        }
        else {
            n += c;
        }
    }
    if(n) t.push(n)
    return t;
}

/**
 * 
 * @param {string[]} tokens 
 */

const utilizeAssembly = (tokens) => {
    let nd = [];
    for(let i = 0; i < tokens.length; i++) {
        if(tokens[i] == '"') {
            let f = false;
            let cs = '';
            i++;
            let d = tokens[i++];
            while(!f) {
                if(d != '"') {
                    if(d == ',') cs += d;
                    else {
                    cs += " " +d;
                    
                    }
                    d = tokens[i++];
                } else f = true;
            }
            nd.push('"'+cs.slice(1)+'"');
        }
        else if(tokens[i] == '\'') {
            if(tokens[i+1] == '\'') {
                nd.push('\' \'');
                i += 2;
            }
            else if(tokens[i+2] != '\'') throw new TypeError('Compile error, char doesn\'t fit!');
            else {
                nd.push(tokens[i+1]);
                i += 3;
            }
        }
        else if(tokens[i] == '//') {
            // comment detected
            while(tokens[i] != '\n') i++;
        }
        else if(tokens[i] == ',') continue;
        else {
            nd.push(tokens[i]);
        }
    }
    return nd;
}
/**
 * 
 * @param {string} s 
 * @returns {boolean}
 */
const r = s => s.startsWith('R');
/**
 * 
 * @param {string} s 
 * @returns {boolean}
 */
const l = s => s.startsWith('.');
/**
 * 
 * @param {string} s 
 * @returns {boolean}
 */
const p = s => s.startsWith('%');

/**
 * @param {string} s
 * @returns {number}
 */

const ti = s => {
    try {
    if(s.startsWith('.') || s.startsWith('@')) return s;
    else if(s.startsWith('0x')) return parseInt(s.slice(2), 16);
    else if(s.startsWith('0b')) return parseInt(s.slice(2), 2);
    else if(s.startsWith('\'') && s.endsWith('\'')) return s.charCodeAt(1);
    else if(s.startsWith('R') || s.startsWith('%')) return Number(s.slice(1))-1;
    else return Number(s);
    } catch(e) {}
}

class Compiler {
    /**
     * 
     * @param {string[]} tokens 
     */
    constructor(tokens) {
        this.tokens = tokens;
        this.cci = -1;
        this.cti = 0;
        this.output = Array.from({length: 65535}, () => 0);
        this.labels = {};
        this.macro_vars = {};
        this.macro_strs = {};
    }
    /**
     * 
     * @param {number} d 
     */
    p(d) {
        this.output[this.cti] = d;
        this.cti++;
    }
    f() {
        this.cci++;
        return this.tokens[this.cci];
    }
    cl() {
        let o = this.output.reverse();
        let i = 0;
        while(o[i] == 0) {
            i++;
        }
        this.output = o.slice(i).reverse();
    }
    pm() {
        for(let macro in this.macro_vars) {
            if(this.macro_vars[macro].startsWith('.')) {
                this.macro_vars[macro] = this.labels[this.macro_vars[macro].slice(1)].toString();
            }
        }
    }
    po() {
        let no = [];
        for(let i = 0; i < this.output.length; i++) {
            if(typeof this.output[i] == 'string' && typeof this.output[i].startsWith == 'function') {
                if(this.output[i].startsWith('.')) {
                    no.push(this.labels[this.output[i].slice(1)]);
                } else if(this.output[i].startsWith('@')) {
                    if(this.macro_vars[this.output[i].slice(1)])
                        no.push(ti(this.macro_vars[this.output[i].slice(1)]));
                    else if(this.macro_strs[this.output[i].slice(1)]) {
                        let ns = this.macro_strs[this.output[i].slice(1)].slice(1, -1);
                        for(let c of ns) {
                            no.push(c.charCodeAt(0));
                        }
                    }
                }
            } else no.push(this.output[i]);
        }
        this.output = no;
    }
    c() {
        while(true) {
            let d = this.f();
            if(!d) break;
            if(d == 'MOV') {
                let de = this.f();
                let d1 = this.f();
                if(r(de) && r(d1)) {
                    this.p(is.mov);
                    this.p(ti(de));
                    this.p(ti(d1));
                }
            }
            else if(d == 'IMM') {
                let de = this.f();
                let d1 = this.f();
                if(r(de) && !r(d1)) {
                    this.p(is.imm);
                    this.p(ti(de));
                    this.p(ti(d1));
                }
            }
            else if(d == 'ADD') {
                let de = this.f();
                let d1 = this.f();
                let d2 = this.f();
                if(r(d1) && r(d2)) {
                    this.p(is.addrr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(r(d1) && !r(d2)) {
                    this.p(is.addri);
                    this.p(ti(de));
                    this.p(ti(d1))
                    this.p(ti(d2));
                } else if(!r(d1) && r(d2)) {
                    this.p(is.addir);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else {
                    throw new Error('Compile Error, wrong operands for ADD instruction');
                }
            }
            else if(d == 'SUB') {
                let de = this.f();
                let d1 = this.f();
                let d2 = this.f();
                if(r(d1) && r(d2)) {
                    this.p(is.subrr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(r(d1) && !r(d2)) {
                    this.p(is.subri);
                    this.p(ti(de));
                    this.p(ti(d1))
                    this.p(ti(d2));
                } else if(!r(d1) && r(d2)) {
                    this.p(is.subir);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else {
                    throw new Error('Compile Error, wrong operands for SUB instruction');
                }
            }
            else if(d == 'XOR') {
                let de = this.f();
                let d1 = this.f();
                let d2 = this.f();
                if(r(d1) && r(d2)) {
                    this.p(is.xorrr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(r(d1) && !r(d2)) {
                    this.p(is.xorlr);
                    this.p(ti(de));
                    this.p(ti(d2));
                    this.p(ti(d1));
                } else if(!r(d1) && r(d2)) {
                    this.p(is.xorlr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else {
                    throw new Error('Compile Error, wrong operands for XOR instruction');
                }
            }
            else if(d == 'OR') {
                let de = this.f();
                let d1 = this.f();
                let d2 = this.f();
                if(r(d1) && r(d2)) {
                    this.p(is.orrr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(r(d1) && !r(d2)) {
                    this.p(is.orlr);
                    this.p(ti(de));
                    this.p(ti(d1))
                    this.p(ti(d2));
                } else if(!r(d1) && r(d2)) {
                    this.p(is.orlr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else {
                    throw new Error('Compile Error, wrong operands for OR instruction');
                }
            }
            else if(d == 'AND') {
                let de = this.f();
                let d1 = this.f();
                let d2 = this.f();
                if(r(d1) && r(d2)) {
                    this.p(is.andrr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(r(d1) && !r(d2)) {
                    this.p(is.andlr);
                    this.p(ti(de));
                    this.p(ti(d1))
                    this.p(ti(d2));
                } else if(!r(d1) && r(d2)) {
                    this.p(is.andlr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else {
                    throw new Error('Compile Error, wrong operands for AND instruction');
                }
            }
            else if(d == 'RSH') {
                let de = this.f();
                let d1 = this.f();
                this.p(is.rshr);
                this.p(ti(de));
                this.p(ti(d1));
            }
            else if(d == 'LSH') {
                let de = this.f();
                let d1 = this.f();
                this.p(is.lshr);
                this.p(ti(de));
                this.p(ti(d1));
            }
            else if(d == 'INC') {
                let de = this.f();
                let d1 = this.f();
                this.p(is.incr);
                this.p(ti(de));
                this.p(ti(d1));
            }
            else if(d == 'DEC') {
                let de = this.f();
                let d1 = this.f();
                this.p(is.decr);
                this.p(ti(de));
                this.p(ti(d1));
            }
            else if(d == 'NOT') {
                let de = this.f();
                let d1 = this.f();
                this.p(is.notr);
                this.p(ti(de));
                this.p(ti(d1));
            }
            else if(d == 'LOD') {
                let de = this.f();
                let d1 = this.f();
                if(r(d1)) {
                    this.p(is.lodr);
                    this.p(ti(de));
                    this.p(ti(d1));
                }
                else if(!r(d1)) {
                    this.p(is.lodl);
                    this.p(ti(de));
                    this.p(ti(d1));
                }
            }
            else if(d == 'STR') {
                let de = this.f();
                let d1 = this.f();
                if(r(de) && r(d1)) {
                    this.p(is.strrr);
                    this.p(ti(de));
                    this.p(ti(d1));
                } else if(!r(de) && r(d1)) {
                    this.p(is.strar);
                    this.p(ti(de));
                    this.p(ti(d1));
                } else if(!r(de) && !r(d1)) {
                    this.p(is.stral);
                    this.p(ti(de));
                    this.p(ti(d1));
                } else if(r(de) && !r(d1)) {
                    this.p(is.strrl);
                    this.p(ti(de));
                    this.p(ti(d1));
                }
            }
            else if(d.startsWith('@')) {
                // macros things
                let macro = d.slice(1);
                if(macro == 'org') {
                    this.cti = ti(this.f());
                } else if(macro == 'define') {
                    let vname = this.f();
                    let maData = '';
                    let ldata = '';
                    while(!(ldata == '\n' || ldata == ';')) {
                        maData += ldata;
                        ldata = this.f();
                    }
                    this.macro_vars[vname] = maData;
                }
                else if(macro == 'string') {
                    let vname = this.f();
                    let maData = '';
                    let ldata = '';
                    while(!(ldata == '\n' || ldata == ';')) {
                        maData += ldata;
                        ldata = this.f();
                    }
                    if(maData.startsWith('"') && maData.endsWith('"')) {
                        this.macro_strs[vname] = maData;
                    }
                }
            }
            else if(d == 'JMP') {
                let de = this.f();
                if(r(de)) {
                    this.p(is.brar);
                    this.p(ti(de));
                } else {
                    this.p(is.bral);
                    this.p(ti(de));
                }
            }
            
            else if(d.startsWith('B') && (Object.keys(is).includes(d.toLowerCase()+'r'))) {
                let de = this.f();
                if(r(de)) {
                    this.p(is[d.toLowerCase()+'r']);
                    this.p(ti(de));
                } else {
                    this.p(is[d.toLowerCase()+'l']);
                    this.p(ti(de));
                }
            } else if(d.startsWith('B') && (Object.keys(is).includes(d.toLowerCase()+'rrr'))) {
                let de = this.f();
                let d1 = this.f();
                let d2 = this.f();
                if(r(de)) {
                    if(r(d1) && r(d2)) {
                        this.p(is[d.toLowerCase()+'rrr']);
                        this.p(ti(de));
                        this.p(ti(d1));
                        this.p(ti(d2));
                    }
                    else if(r(d1) && !r(d2)) {
                        this.p(is[d.toLowerCase()+'rrl'])
                        this.p(ti(de));
                        this.p(ti(d1));
                        this.p(ti(d2));
                    }
                    else if(!r(d1) && r(d2)) {
                        this.p(is[d.toLowerCase()+'rlr'])
                        this.p(ti(de));
                        this.p(ti(d1));
                        this.p(ti(d2));
                    }
                } else {
                    if(r(d1) && r(d2)) {
                        this.p(is[d.toLowerCase()+'lrr']);
                        this.p(ti(de));
                        this.p(ti(d1));
                        this.p(ti(d2));
                    }
                    else if(r(d1) && !r(d2)) {
                        this.p(is[d.toLowerCase()+'lrl'])
                        this.p(ti(de));
                        this.p(ti(d1));
                        this.p(ti(d2));
                    }
                    else if(!r(d1) && r(d2)) {
                        this.p(is[d.toLowerCase()+'llr'])
                        this.p(ti(de));
                        this.p(ti(d1));
                        this.p(ti(d2));
                    }
                }
            }
            else if(d == 'DW') {
                let d2 = this.f();
                if(d2.startsWith('"') && d2.endsWith('"')) {
                    let d3 = d2.slice(1, -1);
                    for(let c of d3) {
                        this.p(c.charCodeAt(0));
                    }
                } else if(d2.startsWith("'") && d2.startsWith("'")) this.p(d2.charCodeAt(1));
                else {
                    this.p(ti(d2));
                }
            }
            else if(d == 'MLT') {
                let de = this.f();
                let d1 = this.f();
                let d2 = this.f();
                if(r(d1) && r(d2)) {
                    this.p(is.mltrr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(r(d1) && !r(d2)) {
                    this.p(is.mltlr);
                    this.p(ti(de));
                    this.p(ti(d2));
                    this.p(ti(d1));
                }
            }
            else if(d == 'DIV') {
                let de = this.f();
                let d1 = this.f();
                let d2 = this.f();
                if(r(d1) && r(d2)) {
                    this.p(is.divrr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(r(d1) && !r(d2)) {
                    this.p(is.divrl);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(!r(d1) && r(d2)) {
                    this.p(is.divlr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                }
            }
            else if(d == 'MOD') {
                let de = this.f();
                let d1 = this.f();
                let d2 = this.f();
                if(r(d1) && r(d2)) {
                    this.p(is.modrr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(r(d1) && !r(d2)) {
                    this.p(is.modrl);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                } else if(!r(d1) && r(d2)) {
                    this.p(is.modlr);
                    this.p(ti(de));
                    this.p(ti(d1));
                    this.p(ti(d2));
                }
            }
            else if(d.startsWith('.')) {
                this.labels[d.slice(1)] = this.cti;
            }
            else if(d == 'IN') {
                let de = this.f()
                let po = this.f();
                if(r(de) && p(po)) {
                    this.p(is.in);
                    this.p(ti(po));
                    this.p(ti(de));
                }
            } else if(d == 'OUT') {
                let po = this.f();
                let d1 = this.f();
                if(p(po) && r(d1)) {
                    this.p(is.outr);
                    this.p(ti(po));
                    this.p(ti(d1));
                } else if(p(po) && !r(d1)) {
                    this.p(is.outl);
                    this.p(ti(po));
                    this.p(ti(d1));
                }
            }
            else if(d == 'BOD') {
                // if odd
                let de = this.f();
                let r_ = this.f();
                if(!r(de) && r(r_)) {
                    this.p(is.bodl);
                    this.p(ti(de));
                    this.p(ti(r_));
                }
                else if(r(de) && r(r_)) {
                    this.p(is.bodr);
                    this.p(ti(de));
                    this.p(ti(r_));
                }
            }
            else if(d == 'BEV') {
                // if even
                let de = this.f();
                let r_ = this.f();
                if(!r(de) && r(r_)) {
                    this.p(is.bevl);
                    this.p(ti(de));
                    this.p(ti(r_));
                }
                else if(r(de) && r(r_)) {
                    this.p(is.bevr);
                    this.p(ti(de));
                    this.p(ti(r_));
                }
            }
            else if(d == 'PSH') {
                let de = this.f();
                if(r(de)) {
                    this.p(is.pshr);
                    this.p(ti(de));
                } else {
                    this.p(is.pshl);
                    this.p(ti(de));
                }
            } else if(d == 'POP') {
                let de = this.f();
                if(r(de)) {
                    this.p(is.popi);
                    this.p(ti(de));
                }
            }
            else if(d == 'CAL') {
                let de = this.f();
                if(r(de)) {
                    this.p(is.calr);
                    this.p(ti(de));
                } else {
                    this.p(is.call);
                    this.p(ti(de));
                }
            }
            else if(d == 'SAV') {
                let de = this.f();
                if(r(de)) {
                    this.p(is.sav);
                    this.p(ti(de));
                }
            } else if(d == 'RSR') {
                let de = this.f();
                if(r(de)) {
                    this.p(is.rsr);
                    this.p(ti(de));
                }
            }
            else if(d == "RET") this.p(is.ret);
            else if(d == 'HLT') this.p(is.hlt);
        }
        this.cl();
        this.pm();
        for(let i = 0; i < 0xff; i++)
            this.po();
        return this.output;
    }
};

function toLE(o) {
    let ne = [];
    for(let i of o) {
        ne.push(i & 0xff);
        ne.push((i >> 8) & 0xff);
    }
    return ne;
}


let data = new Compiler(utilizeAssembly(split(fs.readFileSync(process.argv[2], 'utf-8'))));
let data2 = Buffer.from(toLE(data.c()));
fs.writeFileSync(process.argv[3], data2);