const categories = {
    pseudo: 0,
    solid: 1,
    brittle: 2,
    liquid: 3,
    gas: 4,
    electronics: 5,
    explosives: 6,
    nuclear: 7,
    hidden: 8
}

const catSymbols = [, '\u0006', '\u0005', '\u0004', '\u0007', '\u0002', '\u0003', '\u0001'];

const lookup = {
    'VOID': {
        id: 0,
        category: categories.pseudo,
        description: 'gender'
    },
    'EMPTY': {
        id: 1,
        category: categories.pseudo
    },

    'SAND': {
        category: categories.brittle,
        description: 'the primordial element, turns into glass when heated'
    },
    'DIRT': {
        category: categories.brittle,
        description: 'its... dirt'
    },
    'COAL': {
        category: categories.brittle,
        description: 'burns predictably'
    },
    'SNOW': {
        category: categories.brittle,
        description: 'beautiful'
    },
    'COBBLESTONE': {
        category: categories.brittle,
        description: 'heavy'
    },
    'SEED': {
        category: categories.brittle,
        description: 'grows into plant on sand or dirt'
    },

    'STONE': {
        category: categories.solid,
        description: 'strong insolator'
    },
    'WOOD': {
        category: categories.solid,
        description: 'helps plants grow, turns into coal when burned'
    },
    'TITANIUM': {
        category: categories.solid,
        description: 'immune to acid, very explosion resistent'
    },
    'PLANT': {
        category: categories.solid,
        description: 'flammable, decomposes into dirt'
    },
    'ICE': {
        category: categories.solid,
        description: 'cold'
    },
    'GLASS': {
        category: categories.solid,
        description: 'refracts light'
    },
    'FUNGUS': {
        category: categories.solid,
        description: 'spreads through solids'
    },

    'WATER': {
        category: categories.liquid,
        description: 'freezes when cold, turns to steam when hot'
    },
    'CEMENT': {
        category: categories.liquid,
        description: 'cures into stone'
    },
    'OIL': {
        category: categories.liquid,
        description: 'burns with a lot of smoke'
    },
    'MAGMA': {
        category: categories.liquid,
        description: 'very hot.'
    },
    'ACID': {
        category: categories.liquid,
        description: 'destroys most materials, creates toxic smoke'
    },
    'LIQUIDNITROGEN': {
        category: categories.liquid,
        description: 'very cold, evaporates on contact',
        name: 'nitrogen'
    },

    'STEAM': {
        category: categories.gas,
        description: 'precipitates water or snow'
    },
    'SMOKE': {
        category: categories.gas,
        description: 'chokes flames'
    },
    'FOG': {
        category: categories.gas,
        description: 'exists until it doesnt'
    },
    'TOXIC': {
        category: categories.gas,
        description: 'flammable, very light'
    },

    'SPARK': {
        category: categories.electronics,
        description: 'electric spark'
    },
    'BATTERY': {
        category: categories.electronics,
        description: 'produces infinite electricity'
    },
    'COPPER': {
        category: categories.electronics,
        description: 'conducts electricity, heats up'
    },
    'TORCH': {
        category: categories.electronics,
        description: 'creates fire when powered'
    },
    'CONVEYER': {
        category: categories.electronics,
        description: 'moves material in the direction it conducts'
    },
    'PISTON': {
        category: categories.electronics,
        description: 'extends to the left, completes circuits'
    },
    'PUMP': {
        category: categories.hidden,
        description: 'doesnt work,,,'
    },
    'LIGHTNING': {
        category: categories.electronics,
        description: 'discharges into the ground, destructive'
    },

    'FIRE': {
        category: categories.explosives,
        description: 'hot, ignites flamamble material'
    },
    'FUSE': {
        category: categories.explosives,
        description: 'burns slowly, sparks'
    },
    'GUNPOWDER': {
        category: categories.explosives,
        description: 'cute and valid'
    },
    'DYNAMITE': {
        category: categories.explosives,
        description: 'stable, detonates when sparked'
    },
    'FIREWORK': {
        category: categories.explosives,
        description: 'shoots up into the air'
    },
    'THERMITE': {
        category: categories.explosives,
        description: 'burns hot and violently'
    },
    'NITRO': {
        category: categories.explosives,
        description: 'highly volatile, very destructive'
    },

    'ELECTRON': {
        category: categories.nuclear,
        description: 'messes with the internals of material'
    },
    'PROTON': {
        category: categories.nuclear,
        description: 'heats elements up'
    },
    'NEUTRON': {
        category: categories.nuclear,
        description: 'changes atomic makeup of material'
    },
    'PHOTON': {
        category: categories.nuclear,
        description: 'pretty colors,,,,'
    },
    'POLONIUM': {
        category: categories.nuclear,
        description: 'decays into neutrons and plutonium'
    },
    'PLUTONIUM': {
        category: categories.nuclear,
        description: 'decays quickly, detonates when hot'
    },
    'CLONER': {
        category: categories.nuclear,
        description: 'copies nearby elements'
    },
    'UNBREAKABLECLONER': {
        name: 'cloner++',
        category: categories.nuclear,
        description: 'harder, better, faster, stronger'
    },

    'PELLET': {
        category: categories.hidden,
    },
    'DEBRIS': {
        category: categories.hidden,
    },
}

function readcstr(ptr) {
    const a = new Uint8Array(wasm.exports.memory.buffer, ptr);
    let i = 0;
    let str = ''
    while(a[i]) str += String.fromCharCode(a[i++]);
    return str; 
}

const enumToString = ['VOID', 'EMPTY'];

function readEnumArray() {
    const charPtrs = new Uint32Array(wasm.exports.memory.buffer, wasm.exports.enumToString.value);
    let i = 0;
    while(charPtrs[i]) {
        const key = readcstr(charPtrs[i++]);
        lookup[key].id = i + 1;
        enumToString[i + 1] = key;
    }
}