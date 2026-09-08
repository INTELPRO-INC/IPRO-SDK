#!/usr/bin/env python3
"""
Copyright (c) 2026 IntelPro Technology Co., Ltd.
All rights reserved.

This software and associated documentation files (the "Software") are
proprietary and confidential to IntelPro Technology Co., Ltd..

Access to and use of this Software is limited exclusively to authorized 
customers and partners of IntelPro Technology Co., Ltd. who have entered into a 
valid license or non-disclosure agreement.

Unauthorized copying, modification, distribution, or reverse engineering 
of this Software, via any medium, is strictly prohibited without 
prior written permission from IntelPro Technology Co., Ltd..

SPDX-License-Identifier: Proprietary
"""

import os
import sys
import json
import kconfiglib

def expr_to_str(expr):
    if expr is None:
        return "y"
    if isinstance(expr, tuple):
        op = expr[0]
        if op == kconfiglib.AND:
            return " && ".join(expr_to_str(e) for e in expr[1:])
        if op == kconfiglib.OR:
            return " || ".join(expr_to_str(e) for e in expr[1:])
        if op == kconfiglib.NOT:
            return "!" + expr_to_str(expr[1])
        if op == kconfiglib.EQUAL:
            return f"{expr_to_str(expr[1])} = {expr_to_str(expr[2])}"
        if op == kconfiglib.UNEQUAL:
            return f"{expr_to_str(expr[1])} != {expr_to_str(expr[2])}"
    if isinstance(expr, kconfiglib.Symbol):
        return expr.name
    return str(expr)

def collect_nodes(menu_node, proj_conf_values, seen=set()):
    result = []
    while menu_node:
        item = menu_node.item
        prompt = menu_node.prompt[0] if menu_node.prompt else ""
        depends_on = expr_to_str(menu_node.dep)

        if isinstance(item, kconfiglib.Symbol):
            if item.name in seen and not item.choice:
                menu_node = menu_node.next
                continue
            
            seen.add(item.name)
            visible = bool(menu_node.prompt)
            prompt_str = menu_node.prompt[0] if menu_node.prompt else f"[{item.name}]"            
            node = {
                "name": item.name,
                "prompt": prompt_str,
                "type": kconfiglib.TYPE_TO_STR[item.type],
                "value": proj_conf_values.get(item.name, item.str_value),
                "depends_on": expr_to_str(menu_node.dep),
                "visible": bool(visible),
                "hidden_reason": "" if visible else "no prompt or depends condition not met",
                "children": [],
            }
            if item.choice:
                node["parentIsChoice"] = True
            if menu_node.list:
                node["children"] = collect_nodes(menu_node.list, proj_conf_values, seen)
            result.append(node)

        elif isinstance(item, kconfiglib.Choice):
            node = {
                "name": f"CHOICE_{id(item)}",
                "prompt": prompt,
                "type": "choice",
                "value": item.selection.name if item.selection else "",
                "depends_on": depends_on,
                "children": []
            }
            for sym in item.syms:
                node["children"].append({
                    "name": sym.name,
                    "prompt": sym.nodes[0].prompt[0] if sym.nodes[0].prompt else f"[{sym.name}]",
                    "type": kconfiglib.TYPE_TO_STR[sym.type],
                    "value": proj_conf_values.get(sym.name, sym.str_value),
                    "depends_on": "<choice>",
                    "children": [],
                    "parentIsChoice": True
                })
            result.append(node)

        elif item == kconfiglib.MENU:
            node = {
                "name": "",
                "prompt": prompt,
                "type": "menu",
                "value": "",
                "depends_on": depends_on,
                "children": collect_nodes(menu_node.list, proj_conf_values, seen)
            }
            result.append(node)

        elif item == kconfiglib.COMMENT:
            node = {
                "name": "",
                "prompt": prompt,
                "type": "comment",
                "value": "",
                "depends_on": depends_on,
                "children": []
            }
            result.append(node)

        menu_node = menu_node.next
    return result

def parse_proj_conf(proj_conf_path):
    result = {}
    with open(proj_conf_path, 'r') as f:
        for line in f:
            line = line.strip()
            if line.startswith('# CONFIG_') and 'is not set' in line:
                key = line.split()[1]
                result[key.replace('CONFIG_', '')] = 'n'
            elif line.startswith('CONFIG_'):
                key, val = line.split('=', 1)
                key = key.replace('CONFIG_', '')  # ⬅️ 去掉 CONFIG_
                if val in ['y', 'n']:
                    result[key] = val
                else:
                    result[key] = val.strip('"')
    return result

def main():
    if len(sys.argv) < 2:
        print("Usage: parse_kconfig_dynamic.py Kconfig [PROJECT_SYMBOL]")
        sys.exit(1)

    kconfig_file = sys.argv[1]
    project_conf = sys.argv[2] if len(sys.argv) >= 3 else None

    # os.environ["ARCH"] = "ipro"  # for compatibility if needed
    os.environ["srctree"] = os.path.dirname(os.path.abspath(kconfig_file))
    kconf = kconfiglib.Kconfig(kconfig_file)

    if project_conf:
        project_conf_values = parse_proj_conf(project_conf)
    else:
        project_conf_values = {}

    tree = {
        "configs": collect_nodes(kconf.top_node, project_conf_values)
    }

    print(json.dumps(tree, indent=2))

if __name__ == "__main__":
    main()
